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

  EffectParameter<float> ui_[COUNT]; ///< UIから設定するパラメータ
  signalSw bypass;
  sawWave saw;
  sineWave sin;
  triangleWave tri;
  float level_;
  int type_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 更新対象のパラメータ番号
  void update(uint32_t n)
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

public:
  /// @brief コンストラクタ
  Oscillator() //
      : ui_({
            EffectParameter<float>(0, 100, 1, "LEVEL"), //
            EffectParameter<float>(2, 200, 2, "FREQ"),  //
            EffectParameter<float>(0, 2, 1, "TYPE"),    //
        }),                                             //
        level_(0),                                      //
        type_(0)                                        //
  {
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      update(n);
    }
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
      fx = level_ * fx; // LEVEL
      right[i] = bypass.process(right[i], fx, true);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "Oscillator";
    strcpy(buf, name);
    return sizeof(name);
  }
  /// @brief パラメータ数を取得
  /// @return パラメータ数
  uint32_t getParamCount() const noexcept override { return COUNT; }
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  void incrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].increment();
      update(n);
    }
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  void decrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].decrement();
      update(n);
    }
  }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] ratio 比率（0.0f 〜 1.0f）
  void setParam(uint32_t n, float ratio) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].setValue(ratio);
      update(n);
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
    case FREQ:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[n].getValue())));
    case TYPE:
    {
      char const *const typeName[] = {"SAW", "TRI", "SIN"};
      char const *dst = typeName[type_];
      strcpy(buf, dst);
      return strlen(dst);
    }
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x00, 0x08, 0x20}; }
};
