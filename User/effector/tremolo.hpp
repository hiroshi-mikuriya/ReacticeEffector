/// @file      effector/tremolo.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_calc.hpp"
#include "lib/lib_osc.hpp"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class Tremolo;
}
} // namespace satoh

/// @brief Sodiumから拝借したトレモロ
class satoh::fx::Tremolo : public satoh::fx::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    RATE,      ///< 周期
    DEPTH,     ///< 深さ
    WAVE,      ///< 波形
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  float level_;                ///< レベル
  float wave_;                 ///< 波形
  float depth_;                ///< 深さ
  triangleWave tri;            ///< 周期

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -20.0f, 20.0f); // LEVEL -20～20 dB
      break;
    case RATE:
    {
      float rate = 0.01f * (105.0f - ui_[RATE].getValue()); // RATE 周期 1.05～0.05 秒
      tri.set(1.0f / rate);                                 // 三角波 周波数設定
      break;
    }
    case DEPTH:
      depth_ = ui_[DEPTH].getValue() * 0.1f; // DEPTH -10～10 dB
      break;
    case WAVE:
      wave_ = logPot(ui_[WAVE].getValue(), 0.0f, 50.0f); // WAVE 三角波～矩形波変形
      break;
    }
  }
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override
  {
    switch (n)
    {
    case LEVEL:
    case RATE:
    case WAVE:
    case DEPTH:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Tremolo()                                                            //
      : EffectorBase(TREMOLO, "Tremolo", "TR", RGB{0x00, 0x20, 0x00}), //
        ui_({
            EffectParameterF(1, 100, 1, "LEVEL"), //
            EffectParameterF(1, 100, 1, "RATE"),  //
            EffectParameterF(1, 100, 1, "DEPTH"), //
            EffectParameterF(1, 100, 1, "WAVE"),  //
        }),                                       //
        level_(0),                                //
        wave_(0),                                 //
        depth_(0)                                 //
  {
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~Tremolo() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      float gain = 2.0f * tri.output() - 1.0f; // LFO -1～1 三角波
      gain *= wave_;                           // 三角波を増幅
      satoh::fx::compress(-1.0f, gain, 1.0f);  // クリッピング（矩形波に近い形へ）
      gain = depth_ * gain;                    // DEPTH -10～10 dB
      fx *= dbToGain(gain);                    // 音量を揺らす
      fx *= level_;                            // LEVEL
      right[i] = fx;
    }
  }
};
