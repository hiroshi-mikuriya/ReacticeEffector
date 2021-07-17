/// @file      effector/oscillator.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_calc.hpp"
#include "lib_osc.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class Oscillator;
}

/// @brief Sodiumから拝借したオシレーター
class satoh::Oscillator : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0,
    FREQ,
    TYPE,
    COUNT, ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  sawWave saw;
  sineWave sin;
  triangleWave tri;
  float level_;
  int type_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -50.0f, 0.0f); // LEVEL -50～0 dB;
      break;
    case FREQ:
    {
      float freq = 10.0f * ui_[FREQ].getValue(); // 周波数 20～2000 Hz;
      saw.set(freq);
      tri.set(freq);
      sin.set(freq);
      break;
    }
    case TYPE:
      type_ = static_cast<int>(ui_[TYPE].getValue());
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
    case FREQ:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    case TYPE:
    {
      constexpr char const *typeName[] = {"SAW", "TRI", "SIN"};
      return typeName[type_];
    }
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Oscillator()                                                                   //
      : EffectorBase(fx::OSCILLATOR, "Oscillator", "OS", RGB{0x00, 0x08, 0x20}), //
        ui_({
            EffectParameterF(0, 100, 1, "LEVEL"), //
            EffectParameterF(2, 200, 2, "FREQ"),  //
            EffectParameterF(0, 2, 1, "TYPE"),    //
        }),                                       //
        level_(0),                                //
        type_(0)                                  //
  {
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~Oscillator() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      switch (type_)
      {
      case 0:
        fx = 2.0f * saw.output() - 1.0f;
        break;
      case 1:
        fx = 2.0f * tri.output() - 1.0f;
        break;
      case 2:
        fx = sin.output();
        break;
      }
      fx *= level_; // LEVEL
      right[i] = fx;
    }
  }
};
