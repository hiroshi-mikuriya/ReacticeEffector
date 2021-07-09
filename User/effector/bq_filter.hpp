/// @file      effector/bq_filter.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_calc.hpp"
#include "lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class BqFilter;
}

/// @brief Sodiumから拝借したBQフィルター
class satoh::BqFilter : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0,
    TYPE,
    FREQ,
    Q,
    GAIN,
    COUNT, ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  signalSw bypass;
  biquadFilter bqf1;
  float level_;
  int type_;
  float freq_;
  float q_;
  float gain_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint32_t n)
  {
    switch (n)
    {
    case LEVEL:
      level_ = dbToGain(ui_[LEVEL].getValue()); // LEVEL -20...+20 dB
      break;
    case TYPE:
      type_ = static_cast<int>(ui_[TYPE].getValue()); // フィルタタイプ
      bqf1.setCoef(type_, freq_, q_, gain_);          // フィルタ 係数設定
      break;
    case FREQ:
      freq_ = ui_[FREQ].getValue() * 10.0f;  // フィルタ 周波数 20...9990 Hz
      bqf1.setCoef(type_, freq_, q_, gain_); // フィルタ 係数設定
      break;
    case Q:
      q_ = ui_[Q].getValue() * 0.1f;         // フィルタ Q 0.1...9.9
      bqf1.setCoef(type_, freq_, q_, gain_); // フィルタ 係数設定
      break;
    case GAIN:
      gain_ = ui_[GAIN].getValue();          // フィルタ GAIN -15...+15 dB
      bqf1.setCoef(type_, freq_, q_, gain_); // フィルタ 係数設定
      break;
    }
  }

public:
  /// @brief コンストラクタ
  BqFilter() //
      : ui_({
            EffectParameterF(-20, 20, 1, "LV"), //
            EffectParameterF(0, 8, 1, "TYPE"),  //
            EffectParameterF(2, 999, 10, "Fc"), //
            EffectParameterF(1, 99, 1, "Q"),    //
            EffectParameterF(-15, 15, 1, "dB"), //
        }),                                     //
        level_(0),                              //
        type_(0),                               //
        freq_(0),                               //
        q_(0),                                  //
        gain_(0)                                //
  {
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
  }
  /// @brief デストラクタ
  virtual ~BqFilter() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      fx = bqf1.process(fx); // フィルタ実行
      fx = level_ * fx;      // LEVEL
      right[i] = bypass.process(right[i], fx, true);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "FX Template";
    strcpy(buf, name);
    return sizeof(name);
  }
  /// @brief パラメータ数を取得
  /// @return パラメータ数
  uint32_t getParamCount() const noexcept override { return COUNT; }
  /// @brief パラメータ取得
  /// @param[in] n 取得対象のパラメータ番号
  float getParam(uint32_t n) const noexcept override { return ui_[n].getValue(); }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] v 値
  void setParam(uint32_t n, float v) noexcept { ui_[n].setValue(v); }
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  void incrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].increment();
      convUiToFx(n);
    }
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  void decrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].decrement();
      convUiToFx(n);
    }
  }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] ratio 比率（0.0f 〜 1.0f）
  void setParamRatio(uint32_t n, float ratio) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].setValue(ratio);
      convUiToFx(n);
    }
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
    case LEVEL:
    case GAIN:
    {
      int v = static_cast<int>(ui_[n].getValue());
      if (v <= 0)
      {
        return static_cast<uint32_t>(sprintf(buf, "%d", v));
      }
      else
      {
        return static_cast<uint32_t>(sprintf(buf, "+%d", v));
      }
    }
    case TYPE:
    {
      char const *const typeName[] = {"OFF", "PF", "LSF", "HSF", "LPF", "HPF", "BPF", "NF", "APF"};
      char const *dst = typeName[type_];
      strcpy(buf, dst);
      return strlen(dst);
    }
    case FREQ:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(10 * ui_[FREQ].getValue())));
      break;
    case Q:
      return sprintf(buf, "%f", ui_[Q].getValue() / 10.0f);
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x00, 0x20, 0x20}; }
};
