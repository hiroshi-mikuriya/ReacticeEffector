/// @file      effector/delay.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_calc.hpp"
#include "lib_delay.hpp"
#include "lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class Delay;
}

/// @brief Sodiumから拝借したディレイ
class satoh::Delay : public satoh::EffectorBase
{
  enum
  {
    DTIME,
    ELEVEL,
    FBACK,
    TONE,
    TRAIL,
    TAPDIV,
    COUNT, ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  signalSw bypassIn;
  signalSw bypassOut;
  delayBuf del1;
  lpf2nd lpf2ndTone;
  float dtime_;
  float fback_;
  float elevel_;
  float trail_;
  float divTapTime_;
  float tapTime_;
  int tapDiv_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case DTIME:
      if (ui_[DTIME].getMin() <= divTapTime_ && divTapTime_ < ui_[DTIME].getMax()) // タップテンポ設定時
      {
        dtime_ = divTapTime_;
      }
      else
      {
        dtime_ = ui_[DTIME].getValue(); // DELAYTIME 10 ～ 1500 ms
      }
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
    case TRAIL:
      trail_ = ui_[TRAIL].getValue(); // TRAIL ディレイ音を残す機能 ON OFF
      break;
    case TAPDIV:
    {
      tapDiv_ = static_cast<int>(ui_[TAPDIV].getValue());
      constexpr float tapDivFloat[6] = {1.0f, 1.0f, 0.5f, 0.333333f, 0.75f, 1.0f};
      divTapTime_ = tapTime_ * tapDivFloat[tapDiv_]; // DIV計算済タップ時間
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
    case TRAIL:
      return (ui_[TRAIL].getValue() == 0) ? "OFF" : "ON";
    case TAPDIV:
    {
      constexpr char const *div[] = {"1/1", "1/1", "1/2", "1/3", "3/4", "1/1"};
      return div[tapDiv_];
    }
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Delay()                                                   //
      : EffectorBase("Delay", "DL", RGB{0x20, 0x00, 0x20}), //
        ui_({
            EffectParameterF(10, 100, 5, "TIME"), //
            EffectParameterF(0, 100, 1, "E.LV"),  //
            EffectParameterF(0, 99, 1, "F.BACK"), //
            EffectParameterF(0, 100, 1, "TONE"),  //
            EffectParameterF(0, 1, 1, "TRAIL"),   //
            EffectParameterF(1, 4, 1, "DIV"),     //
        }),
        del1(ui_[DTIME].getMax()), //
        dtime_(0),                 //
        fback_(0),                 //
        elevel_(0),                //
        trail_(0),                 //
        divTapTime_(0),            //
        tapTime_(0),               //
        tapDiv_(0)                 //
  {
    init(ui_, COUNT);
  }
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
      del1.write(fback_ * fx + bypassIn.process(0.0f, right[i], isActive()));
      fx *= elevel_;                                                        // ディレイ音レベル
      right[i] = right[i] + bypassOut.process(trail_ * fx, fx, isActive()); // TRAIL ON時ディレイ音残す
    }
  }
};
