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
  void convUiToFx(uint32_t n)
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
      float tap = ui_[TAPDIV].getValue();
      if (tap < 1)
      {
        ui_[TAPDIV].increment();
      }
      if (4 < tap)
      {
        ui_[TAPDIV].decrement();
      }
      tapDiv_ = static_cast<int>(ui_[TAPDIV].getValue());
      constexpr float tapDivFloat[6] = {1.0f, 1.0f, 0.5f, 0.333333f, 0.75f, 1.0f};
      divTapTime_ = tapTime_ * tapDivFloat[tapDiv_]; // DIV計算済タップ時間
      break;
    }
    }
  }

public:
  /// @brief コンストラクタ
  Delay() //
      : ui_({
            EffectParameterF(10, 1500, 10, "TIM"), //
            EffectParameterF(0, 100, 1, "E.LV"),   //
            EffectParameterF(0, 99, 1, "F.BACK"),  //
            EffectParameterF(0, 100, 1, "TONE"),   //
            EffectParameterF(0, 1, 1, "TRAIL"),    //
            EffectParameterF(0, 5, 1, "DIV"),      //
        }),                                        //
        dtime_(0),                                 //
        fback_(0),                                 //
        elevel_(0),                                //
        trail_(0),                                 //
        divTapTime_(0),                            //
        tapTime_(0),                               //
        tapDiv_(0)                                 //
  {
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
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
      del1.write(fback_ * fx + bypassIn.process(0.0f, right[i], true));
      fx *= elevel_;                                                  // ディレイ音レベル
      right[i] = right[i] + bypassOut.process(trail_ * fx, fx, true); // TRAIL ON時ディレイ音残す
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "Delay";
    strcpy(buf, name);
    return sizeof(name);
  }
  /// @brief パラメータ数を取得
  /// @return パラメータ数
  uint32_t getParamCount() const noexcept override { return COUNT; }
  /// @brief パラメータ取得
  /// @param[in] n 取得対象のパラメータ番号
  /// @return パラメータ
  float getParam(uint32_t n) const noexcept override { return ui_[n].getValue(); }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] v 値
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  bool setParam(uint32_t n, float v) noexcept { return ui_[n].setValue(v); }
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  /// @retval true 加算した
  /// @retval false 最大値に到達しているため加算しなかった
  bool incrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      if (ui_[n].increment())
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  /// @retval true 減算した
  /// @retval false 最小値に到達しているため減算しなかった
  bool decrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      if (ui_[n].decrement())
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ比率設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] ratio 比率（最小値 0.0f 〜 1.0f 最大値）
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  bool setParamRatio(uint32_t n, float ratio) noexcept override
  {
    if (n < COUNT)
    {
      if (ui_[n].setValueRatio(ratio))
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ名文字列取得
  /// @param[in] n パラメータ番号
  /// @param[out] buf 文字列格納先
  /// @return 文字列の長さ
  uint32_t getParamName(uint32_t n, char *buf) const noexcept override
  {
    if (n < COUNT)
    {
      return ui_[n].getName(buf);
    }
    return 0;
  }
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @param[out] buf 文字列格納先
  /// @return 文字列の長さ
  uint32_t getValueTxt(uint32_t n, char *buf) const noexcept override
  {
    switch (n)
    {
    case DTIME:
    case ELEVEL:
    case FBACK:
    case TONE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[n].getValue())));
      break;
    case TRAIL:
      if (ui_[TRAIL].getValue() == 0)
      {
        constexpr char const *OFF = "OFF";
        strcpy(buf, OFF);
        return sizeof(OFF);
      }
      else
      {
        constexpr char const *ON = "ON";
        strcpy(buf, ON);
        return sizeof(ON);
      }
      break;
    case TAPDIV:
    {
      const char *const div[] = {"1/1", "1/1", "1/2", "1/3", "3/4", "1/1"};
      const char *dst = div[tapDiv_];
      strcpy(buf, dst);
      return strlen(dst);
      break;
    }
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x20, 0x00, 0x20}; }
};
