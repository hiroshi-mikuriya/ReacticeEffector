/// @file      effector/delay_base.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class DelayBase;
}
} // namespace satoh

/// @brief ディレイ基底クラス
class satoh::fx::DelayBase : public satoh::fx::EffectorBase
{
protected:
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
  lpf2nd lpf2ndTone_;
  float fback_;
  float elevel_;

private:
  /// @brief DTIMEを更新する
  virtual void updateDtime() noexcept = 0;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param [in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case DTIME:
      updateDtime();
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
  /// @param [in] n パラメータ番号
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
  /// @param [in] id エフェクターID
  /// @param [in] name エフェクター名
  /// @param [in] shortName エフェクター名（短縮）
  /// @param [in] ledColor アクティブ時のLED色
  DelayBase(ID id, const char *name, const char *shortName, RGB const &ledColor) //
      : EffectorBase(id, name, shortName, ledColor),                             //
        ui_{
            EffectParameterF(10, 900, 100, 5, "TIME"), //
            EffectParameterF(0, 100, 1, "E.LV"),       //
            EffectParameterF(0, 99, 1, "F.BACK"),      //
            EffectParameterF(0, 100, 1, "TONE"),       //
        },
        fback_(0), //
        elevel_(0) //
  {
  }
  /// @brief デストラクタ
  virtual ~DelayBase() {}
};
