/// @file      effector/delay.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_calc.hpp"
#include "lib/lib_delay.hpp"
#include "lib/lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class Delay;
}
} // namespace satoh

/// @brief Sodiumから拝借したディレイ
class satoh::fx::Delay : public satoh::fx::EffectorBase
{
  enum
  {
    DTIME,
    ELEVEL,
    FBACK,
    TONE,
    COUNT, ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  delayBufF del1;
  lpf2nd lpf2ndTone;
  float dtime_;
  float fback_;
  float elevel_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case DTIME:
      dtime_ = ui_[DTIME].getValue(); // DELAYTIME 10 ～ 1500 ms
      break;
    case ELEVEL:
      elevel_ = logPot(ui_[ELEVEL].getValue(), -20.0f, 20.0f); // EFFECT LEVEL -20 ～ +20dB
      break;
    case FBACK:
      fback_ = ui_[FBACK].getValue() / 100.0f; // Feedback 0 ～ 99 %
      break;
    case TONE:
    {
      float tone = 800.0f * logPot(ui_[TONE].getValue(), 0.0f, 20.0f); // HI CUT FREQ 800 ～ 8000 Hz
      lpf2ndTone.set(tone);
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
    case DTIME:
    case ELEVEL:
    case FBACK:
    case TONE:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Delay()                                                          //
      : EffectorBase(DELAY, "Delay", "DL", RGB{0x20, 0x00, 0x20}), //
        ui_({
            EffectParameterF(10, 10, 1, "TIME"),  //
            EffectParameterF(0, 100, 1, "E.LV"),  //
            EffectParameterF(0, 99, 1, "F.BACK"), //
            EffectParameterF(0, 100, 1, "TONE"),  //
        }),
        del1(ui_[DTIME].getMax()), //
        dtime_(0),                 //
        fback_(0),                 //
        elevel_(0)                 //
  {
    init(ui_, COUNT);
  }
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  Delay(Delay &&) = default;
  /// @brief move演算子
  /// @param[in] that 移動元
  /// @return 自身の参照
  Delay &operator=(Delay &&) = default;
  /// @brief デストラクタ
  virtual ~Delay() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = del1.read(dtime_); // ディレイ音読込
      fx = lpf2ndTone.process(fx);  // ディレイ音のTONE（ハイカット）
      // ディレイ音と原音をディレイバッファに書込、原音はエフェクトオン時のみ書込
      del1.write(fback_ * fx + right[i]);
      fx *= elevel_; // ディレイ音レベル
      right[i] = right[i] + fx;
    }
  }
};
