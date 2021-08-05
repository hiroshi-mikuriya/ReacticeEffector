/// @file      effector/tuner.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "tuner.h"
#include <algorithm>
#include <cmsis_os.h>

namespace
{
// ギター チューナー(ベースでの動作未確認)
// ブロックサイズ 16, 32, 64 サンプリング周波数 44.1kHz 48kHz を想定
//
// 下記ページのコードを改変して使用
// https://www.cycfi.com/2018/03/fast-and-efficient-pitch-detection-bitstream-autocorrelation/

constexpr float FREQ_A = 440.0f;                                     ///< 基準A音周波数 Hz
constexpr float MIN_FREQ = 70.0f;                                    ///< 最低周波数 ギター 6弦ドロップD音 73Hz
constexpr float MAX_FREQ = 400.0f;                                   ///< 最高周波数 ギター 1弦5フレットA音 440Hz
constexpr float TUNER_SAMPLING_FREQ = satoh::SAMPLING_FREQ;          ///< チューナーでのサンプリング周波数
constexpr uint16_t MIN_PERIOD = TUNER_SAMPLING_FREQ / MAX_FREQ;      ///< 最小周期サンプル区間
constexpr uint16_t MAX_PERIOD = TUNER_SAMPLING_FREQ / MIN_FREQ;      ///< 最大周期サンプル区間
constexpr uint16_t IN_DATA_SIZE = 64 * ((MAX_PERIOD * 2) / 64) + 64; ///< 入力音配列 データ数 64の倍数にする
constexpr uint16_t BIT_STREAM_SIZE = IN_DATA_SIZE / 32;              ///< ビットストリーム配列 データ数
constexpr float NOISE_THRETHOLD = 0.02f;                             ///< ノイズ除去用閾値
constexpr uint32_t CORR_ARRAY_SIZE = IN_DATA_SIZE / 2;               ///< 相関係数配列サイズ

// 周波数算出 --------------------------------------------------------
bool estimateFreq(float const *inData, uint32_t const *corrArray, float &estimatedFreq, float maxCorr, float estimatedIndex)
{
  // <実際より低い音として判定される問題を解決＞
  // 短い周期(高い音)の音が入力されると、周期が数倍の点にも谷が現れる
  // →例)周期4ms(250Hz)の音が12ms(83.3Hz)と判定
  // 実際より長く判定された周期を整数で割って整数倍したもの(1/4, 2/4, 3/4, 1/3, 2/3等)
  // それぞれについて相関値を調べ、整数倍全てに深い谷がある(allStrong)場合の除数を採用する
  uint32_t subThreshold = 0.3f * maxCorr;        // 谷の強さ判定閾値
  uint16_t maxDiv = estimatedIndex / MIN_PERIOD; // 除数最大値
  uint16_t estimatedDiv = 1;                     // 採用する除数
  for (int16_t div = maxDiv; div != 0; div--)
  {
    bool allStrong = true;

    for (uint16_t k = 1; k != div; k++)
    {
      uint16_t subPeriod = estimatedIndex * k / (float)div;
      if (corrArray[subPeriod] > subThreshold)
      {
        allStrong = false;
        break;
      }
    }

    if (allStrong)
    {
      estimatedDiv = div; // 除数決定、最終的な周波数計算時に使用
      break;
    }
  }

  // ゼロクロス（-→+） スタート点 検出
  uint16_t startIndex = 1;
  while (startIndex < IN_DATA_SIZE)
  {
    if (inData[startIndex - 1] <= 0.0f && inData[startIndex] > 0.0f)
    {
      break;
    }
    startIndex++;
  }
  float dy = inData[startIndex] - inData[startIndex - 1]; // 線形補間 y
  float dx1 = -inData[startIndex - 1] / dy;               // 線形補間 x1

  // ゼロクロス（-→+） 目的の点 検出
  uint16_t nextIndex = estimatedIndex - 1;
  while (nextIndex < IN_DATA_SIZE)
  {
    if (inData[nextIndex - 1] <= 0.0f && inData[nextIndex] > 0.0f)
    {
      break;
    }
    nextIndex++;
  }
  dy = inData[nextIndex] - inData[nextIndex - 1]; // 線形補間 y
  float dx2 = -inData[nextIndex - 1] / dy;        // 線形補間 x2

  float estimatedPeriod = (nextIndex - startIndex) + (dx2 - dx1); // 推定周期
  estimatedPeriod = estimatedPeriod / (float)estimatedDiv;        // 予め計算した除数で割る

  // 推定周波数が3回連続で近い値となった時に周波数確定
  static float tmpFreq[3] = {}; // 推定周波数 一時保管用
  static uint8_t n = 0;         // 上記tmpFreqの添字 0→1→2→0で循環させる
  if (estimatedPeriod > (float)MIN_PERIOD)
  {
    tmpFreq[n] = TUNER_SAMPLING_FREQ / estimatedPeriod;
  }

  bool res = false;
  if (tmpFreq[n] * 0.97f < tmpFreq[(n + 1) % 3] && //
      tmpFreq[(n + 1) % 3] < 1.03f * tmpFreq[n] && //
      tmpFreq[n] * 0.97f < tmpFreq[(n + 2) % 3] && //
      tmpFreq[(n + 2) % 3] < 1.03f * tmpFreq[n])
  {
    estimatedFreq = (tmpFreq[n] + tmpFreq[(n + 1) % 3] + tmpFreq[(n + 2) % 3]) / 3.0f;
    res = true;
  }
  n = (n + 1) % 3;
  return res;
}

// 自己相関計算 ------------------------------------------------------------------
void bitstreamAutocorrelation(uint16_t startCnt, uint32_t const *bitData, uint32_t *corrArray, uint32_t &maxCorr, uint32_t &minCorr, uint16_t &estimatedIndex)
{
  // pos：position ビットストリーム配列をズラした位置
  // pos → MIN_PERIOD ～ IN_DATA_SIZE/2 まで相間を計算するが、計算負荷軽減のため分割する
  for (uint16_t pos = startCnt; (pos >= MIN_PERIOD) && (pos < startCnt + satoh::BLOCK_SIZE / 2); pos++)
  {
    uint16_t midBitStreamSize = (BIT_STREAM_SIZE / 2) - 1; // ビットストリーム配列データ数の半分
    uint16_t index = startCnt / 32;                        // ビットストリーム配列の何番目の整数か
    uint16_t shift = startCnt % 32;                        // ビットストリーム配列内の整数 シフト数
    uint32_t corr = 0;                                     // correlation(相間)

    if (shift == 0)
    {
      for (uint16_t i = 0; i < midBitStreamSize; i++)
      { // ^(XOR)、ビットが1になっている所を数え相間を算出
        corr += __builtin_popcount(bitData[i] ^ bitData[i + index]);
      }
    }
    else
    {
      for (uint16_t i = 0; i < midBitStreamSize; i++)
      {
        uint32_t tmp = bitData[i + index] >> shift;    // ビットストリーム配列をズラしたもの
        tmp |= bitData[i + 1 + index] << (32 - shift); // 右シフトしたものと左シフトしたものを合わせる
        corr += __builtin_popcount(bitData[i] ^ tmp);
      }
    }
    shift++;
    if (shift == 32) // shiftが32まで来たらindexを増やす
    {
      shift = 0;
      index++;
    }
    corrArray[pos] = corr;             // correlation(相間) 配列
    maxCorr = std::max(maxCorr, corr); // 最大値を記録
    if (corr < minCorr)
    {
      minCorr = corr;       // 最小値を記録
      estimatedIndex = pos; // 相間が最小となる時の位置→周期計算に利用
    }
  }
}

// 入力音配列をビットストリームへ変換 --------------------------------------------
void bitStreamSet(uint16_t cnt, float x, float *inData, uint32_t *bitData)
{
  uint32_t val = 0;  // 入力音がゼロ以上か判定 0または1
  uint32_t mask = 0; // マスクビット

  // 入力音配列にデータを保存
  inData[cnt] = x;

  // 入力音配列をビットストリーム配列へ変換
  if (inData[cnt] < -NOISE_THRETHOLD)
  {
    val = 0;
  }
  else if (inData[cnt] > 0.0f)
  {
    val = 1;
  }
  mask = 1 << (cnt % 32);
  bitData[cnt / 32] ^= (-val ^ bitData[cnt / 32]) & mask; // -val は 111...111 または 000...000（解説省略）
}
} // namespace

satoh::fx::Tuner::Tuner()                                           //
    : EffectorBase(TEMPLATE, "Tuner", "TN", RGB{0x00, 0x00, 0x00}), //
      lpf_(MAX_FREQ),                                               //
      inData_({}),                                                  //
      inDataCnt_(0),                                                //
      bitStream_({}),                                               //
      arraySel_(0),                                                 //
      corrArray_(makeDmaMem<uint32_t>(CORR_ARRAY_SIZE)),            //
      maxCorr_(0),                                                  //
      minCorr_(UINT32_MAX),                                         //
      estimatedIndex_(MIN_PERIOD),                                  //
      estimatedFreq_(999.0f),                                       //
      lastUpdateTime_(0)                                            //
{
  for (int i = 0; i < 2; ++i)
  {
    inData_[i] = UniquePtr<float>(allocArray<float>(IN_DATA_SIZE));
    bitStream_[i] = UniquePtr<uint32_t>(allocArray<uint32_t>(BIT_STREAM_SIZE));
  }
  init(0, 0);
}

void satoh::fx::Tuner::effect(float *left, float *right, uint32_t size) noexcept
{
  // 配列準備と自己相間の計算が終了したデータを使い周波数を算出、変数初期化 //////////
  if (inDataCnt_ == 0)
  {
    if (estimateFreq(inData_[arraySel_].get(), corrArray_.get(), estimatedFreq_, maxCorr_, estimatedIndex_))
    {
      lastUpdateTime_ = osKernelSysTick();
    }
    maxCorr_ = 0;
    minCorr_ = UINT32_MAX;
    memset(corrArray_.get(), 0, sizeof(*corrArray_) * CORR_ARRAY_SIZE);
  }

  // 自己相関計算 ////////////////////////////////////////////////////////////////
  bitstreamAutocorrelation(inDataCnt_ / 2, bitStream_[(arraySel_ + 1) % 2].get(), corrArray_.get(), maxCorr_, minCorr_, estimatedIndex_);

  // 入力音配列とビットストリーム配列を準備 //////////////////////////////////////
  for (uint32_t i = 0; i < size; i++)
  {
    float fx = lpf_.process(right[i]);
    bitStreamSet(inDataCnt_, fx, inData_[arraySel_].get(), bitStream_[arraySel_].get());
    inDataCnt_++;
    if (inDataCnt_ == IN_DATA_SIZE) // 配列 準備完了
    {
      inDataCnt_ = 0;
      arraySel_ = (arraySel_ + 1) % 2;
    }
    left[i] = right[i] = 0;
  }
}
