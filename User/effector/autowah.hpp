/// @file      effector/autowah.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class AutoWah;
}
} // namespace satoh

/// @brief Sodiumから拝借したオートワウ
class satoh::fx::AutoWah : public satoh::fx::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< LEVEL
    SENS,      ///< SENSITIVITY
    Q,         ///< Q
    HI_F,      ///< HIFREQ
    LOW_F,     ///< LOFREQ
    COUNT      ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  lpf lpf1, lpf2;              ///< エンベロープ用ローパスフィルタ
  biquadFilter bpf1;           ///< ワウ用バンドパスフィルタ
  float logFreqRatio_ = 0.6f;  ///< バンドパスフィルタ中心周波数計算用変数（BPF周波数変化比の常用対数）
  float level_;                ///< LEVEL 0～+30dB
  float sens_;                 ///< SENSITIVITY 0～+50dB
  float q_;                    ///< Q 1.0～9.0
  float hifreq_;               ///< HIGH FREQ BPF中心周波数 最高 1000～9000Hz
  float lofreq_;               ///< LOW FREQ BPF中心周波数 最低 100～900Hz

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param [in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    const float v = ui_[n].getValue();
    switch (n)
    {
    case LEVEL:
      level_ = logPot(v, 0.0f, 30.0f);
      break;
    case SENS:
      sens_ = 0.5f * v;
      break;
    case Q:
      q_ = v / 10.0f;
      break;
    case HI_F:
      hifreq_ = v;
      if (lofreq_ != 0)
      {
        logFreqRatio_ = log10f(hifreq_ / lofreq_);
      }
      break;
    case LOW_F:
      lofreq_ = v;
      if (lofreq_ != 0)
      {
        logFreqRatio_ = log10f(hifreq_ / lofreq_);
      }
      break;
    }
  }
  /// @brief パラメータ値文字列取得
  /// @param [in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override
  {
    float v = ui_[n].getValue();
    switch (n)
    {
    case LEVEL:
    case SENS:
    case HI_F:
    case LOW_F:
    case Q:
      sprintf(valueTxt_, "%d", static_cast<int>(v));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  AutoWah()
      : EffectorBase(AUTO_WAH, "Auto Wah", "AW", RGB{0x20, 0x20, 0x20}), // エフェクター名、短縮エフェクター名、LED色
        ui_{
            EffectParameterF(0, 100, 1, "LEVEL"),     //
            EffectParameterF(0, 100, 1, "SENS"),      //
            EffectParameterF(10, 90, 1, "Q"),         //
            EffectParameterF(1000, 9000, 100, "HiF"), //
            EffectParameterF(100, 900, 10, "LowF"),   //
        },                                            //
        level_(0),                                    //
        sens_(0),                                     //
        q_(0),                                        //
        hifreq_(0),                                   //
        lofreq_(0)                                    //
  {
    lpf1.set(20.0f); // エンベロープ用ローパスフィルタ設定
    lpf2.set(50.0f);
    bpf1.setBPF(1000.0f, 1.0f); // ワウ用バンドパスフィルタ初期値
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~AutoWah() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param [in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      float env = std::abs(fx);
      env = lpf1.process(env);                              // 絶対値とLPFでエンベロープ取得
      env = gainToDb(env);                                  // dB換算
      env = env + sens_;                                    // SENSITIVITY 感度(エンベロープ補正)
      compress(-20.0f, env, 0.0f);                          // -20～0dBまででクリップ
      env = lpf2.process(env);                              // 急激な変化を避ける
      float freq = hifreq_ * dbToGain(logFreqRatio_ * env); // エンベロープに応じた周波数を計算 指数的変化
      bpf1.setBPF(freq, q_);                                // フィルタ周波数を設定
      fx = bpf1.process(fx);                                // フィルタ(ワウ)実行
      fx = level_ * fx;                                     // LEVEL
      right[i] = fx;
    }
  }
};
