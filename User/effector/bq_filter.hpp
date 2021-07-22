/// @file      effector/bq_filter.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_calc.hpp"
#include "lib/lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class BqFilter;
}
} // namespace satoh

/// @brief Sodiumから拝借したBQフィルター
class satoh::fx::BqFilter : public satoh::fx::EffectorBase
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
  mutable char valueTxt_[16];  ///< パラメータ文字列格納バッファ
  biquadFilter bqf1;
  float level_;
  int type_;
  float freq_;
  float q_;
  float gain_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
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
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override
  {
    switch (n)
    {
    case LEVEL:
    case GAIN:
    {
      int v = static_cast<int>(ui_[n].getValue());
      if (v <= 0)
      {
        sprintf(valueTxt_, "%d", v);
      }
      else
      {
        sprintf(valueTxt_, "+%d", v);
      }
      return valueTxt_;
    }
    case TYPE:
    {
      constexpr char const *typeName[] = {"OFF", "PF", "LSF", "HSF", "LPF", "HPF", "BPF", "NF", "APF"};
      return typeName[type_];
    }
    case FREQ:
      sprintf(valueTxt_, "%d", static_cast<int>(10 * ui_[FREQ].getValue()));
      return valueTxt_;
    case Q:
      sprintf(valueTxt_, "%f", ui_[Q].getValue() / 10.0f);
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  BqFilter()                                                               //
      : EffectorBase(BQ_FILTER, "BQ Filter", "BQ", RGB{0x00, 0x20, 0x20}), //
        ui_({
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
    init(ui_, COUNT);
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
      fx *= level_;          // LEVEL
      right[i] = fx;
    }
  }
};
