/// @file      effector/tiny_fuzz.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include <cstdio> // sprintf

namespace satoh
{
class TinyFuzz;
}

/// @brief 実験のために作ったしょぼいファズ（あとで消すと思う）
class satoh::TinyFuzz : public satoh::EffectorBase
{
  enum
  {
    GAIN = 0, ///< ゲイン
    COMP,     ///< 圧縮値
    COUNT,    ///< パラメータ総数
  };

  EffectParameter<float> params_[COUNT]; ///< パラメータ

public:
  /// @brief コンストラクタ
  TinyFuzz() //
      : params_({
            EffectParameter<float>(1, 100, 1, "GAIN"),          //
            EffectParameter<float>(0.05f, 1.0f, 0.05f, "COMP"), //
        })
  {
  }
  /// @brief デストラクタ
  virtual ~TinyFuzz() {}
  /// @brief エフェクト処理実行
  /// @param[inout] v 音声データ
  /// @param[in] size 音声データ数
  void effect(float *v, uint32_t size) noexcept override
  {
    const float gain = params_[GAIN].getValue();
    const float comp = params_[COMP].getValue();
    for (uint32_t i = 0; i < size; ++i)
    {
      v[i] *= gain;
      compress(-comp, v[i], comp);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "Tiny Fuzz";
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
      params_[n].increment();
    }
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  void decrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      params_[n].decrement();
    }
  }
  /// @brief パラメータ設定
  /// @param[in] n 減算対象のパラメータ番号
  /// @param[in] ratio 比率（0.0f 〜 1.0f）
  virtual void setParam(uint32_t n, float ratio) noexcept override
  {
    if (n < COUNT)
    {
      params_[n].setValue(ratio);
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
      return params_[n].getName(buf);
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
    case GAIN:
      return static_cast<uint32_t>(sprintf(buf, "%f", params_[GAIN].getValue()));
    case COMP:
      return static_cast<uint32_t>(sprintf(buf, "%.2f", params_[COMP].getValue()));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x20, 0x00, 0x00}; }
};
