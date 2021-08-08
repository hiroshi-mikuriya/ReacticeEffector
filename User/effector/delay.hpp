/// @file      effector/delay.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_delay.hpp"
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
  delayBuf<int16_t> del1_;
  lpf2nd lpf2ndTone_;
  float fback_;
  float elevel_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case DTIME:
      del1_.setInterval(ui_[DTIME].getValue());
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
      lpf2ndTone_.set(tone);
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
            EffectParameterF(10, 900, 100, 5, "TIME"), //
            EffectParameterF(0, 100, 1, "E.LV"),       //
            EffectParameterF(0, 99, 1, "F.BACK"),      //
            EffectParameterF(0, 100, 1, "TONE"),       //
        }),
        del1_(ui_[DTIME].getMax()), //
        fback_(0),                  //
        elevel_(0)                  //
  {
    if (del1_)
    {
      init(ui_, COUNT);
    }
  }
  /// @brief デストラクタ
  virtual ~Delay() {}
  /// @brief エフェクターセットアップ成功・失敗
  /// @retval true 成功
  /// @retval false 失敗
  explicit operator bool() const noexcept override { return static_cast<bool>(del1_); }
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = del1_.read();
      fx = lpf2ndTone_.process(fx);
      del1_.write(fback_ * fx + right[i]);
      right[i] += fx * elevel_;
    }
  }
};
