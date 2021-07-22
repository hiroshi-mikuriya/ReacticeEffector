/// @file      effector/chorus.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_calc.hpp"
#include "lib/lib_delay.hpp"
#include "lib/lib_filter.hpp"
#include "lib/lib_osc.hpp"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class Chorus;
}
} // namespace satoh

/// @brief Sodiumから拝借したコーラス
class satoh::fx::Chorus : public satoh::fx::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    MIX,       ///< ミックス
    FBACK,     ///< フィードバック
    RATE,      ///< 周期
    DEPTH,     ///< 深さ
    TONE,      ///< トーン
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  sineWave sin1;
  delayBuf<int8_t> del1;
  hpf hpf1;
  lpf2nd lpf2nd1;
  lpf2nd lpf2nd2;
  float level_;
  float mix_;
  float fback_;
  float depth_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -20.0f, 20.0f); // LEVEL -20 ～ 20dB;
      break;
    case MIX:
      mix_ = mixPot(ui_[MIX].getValue(), -20.0f); // MIX;
      break;
    case FBACK:
      fback_ = ui_[FBACK].getValue() / 100.0f; // Feedback 0～99%;
      break;
    case RATE:
    {
      float rate = 0.02f * (105.0f - ui_[RATE].getValue()); // RATE 周期 2.1～0.1 秒;
      sin1.set(1.0f / rate);
      break;
    }
    case DEPTH:
      depth_ = 0.05f * ui_[DEPTH].getValue(); // Depth ±5ms;
      break;
    case TONE:
    {
      float tone = 800.0f * logPot(ui_[TONE].getValue(), 0.0f, 20.0f); // HI CUT FREQ 800 ～ 8000 Hz
      lpf2nd1.set(tone);
      lpf2nd2.set(tone);
      break;
    }
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
    case MIX:
    case FBACK:
    case RATE:
    case DEPTH:
    case TONE:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Chorus()
      : EffectorBase(CHORUS, "Chorus", "CH", RGB{0x00, 0x10, 0x20}), //
        ui_({
            EffectParameterF(1, 100, 1, "LEVEL"), //
            EffectParameterF(1, 100, 1, "MIX"),   //
            EffectParameterF(1, 99, 1, "F.BACK"), //
            EffectParameterF(1, 100, 1, "RATE"),  //
            EffectParameterF(1, 100, 1, "DEPTH"), //
            EffectParameterF(1, 100, 1, "TONE"),  //
        }),                                       //
        del1(16),                                 //
        level_(0),                                //
        mix_(0),                                  //
        fback_(0),                                //
        depth_(0)                                 //
  {
    hpf1.set(100.0f); // ディレイ音のローカット設定
    init(ui_, COUNT);
  }
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  Chorus(Chorus &&) = default;
  /// @brief move演算子
  /// @param[in] that 移動元
  /// @return 自身の参照
  Chorus &operator=(Chorus &&) = default;
  /// @brief デストラクタ
  virtual ~Chorus() {}
  /// @brief エフェクターセットアップ成功・失敗
  /// @retval true 成功
  /// @retval false 失敗
  bool ok() const noexcept override { return del1.ok(); }
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float dtime = 5.0f + depth_ * (1.0f + sin1.output()); // ディレイタイム 5～15ms
      float fx = del1.readLerp(dtime);                      // ディレイ音読込(線形補間)
      fx = lpf2nd1.process(fx);                             // ディレイ音のTONE(ハイカット)
      fx = lpf2nd2.process(fx);
      // ディレイ音と原音をディレイバッファに書込、原音はローカットして書込
      del1.write(fback_ * fx + hpf1.process(right[i]));
      fx = (1.0f - mix_) * right[i] + mix_ * fx; // MIX
      fx *= 1.4f * level_;                       // LEVEL
      right[i] = fx;
    }
  }
};
