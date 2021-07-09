/// @file      effector/effector_template.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include <cstdio> // sprintf

namespace satoh
{
class EffectorTemplate;
}

/// @brief エフェクタークラスの実装参考用テンプレート
class satoh::EffectorTemplate : public satoh::EffectorBase
{
  enum
  {
    PARAM0 = 0, ///< パラメータ0
    PARAM1,     ///< パラメータ1
    PARAM2,     ///< パラメータ2
    COUNT,      ///< パラメータ総数
  };

  EffectParameter<float> ui_[COUNT]; ///< UIから設定するパラメータ
  float param0_;                     ///< エフェクト処理で使用するパラメータ0
  float param1_;                     ///< エフェクト処理で使用するパラメータ1
  float param2_;                     ///< エフェクト処理で使用するパラメータ2

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 更新対象のパラメータ番号
  void update(uint32_t n)
  {
    switch (n)
    {
    case PARAM0:
      param0_ = ui_[PARAM0].getValue(); // TODO 加工して代入する
      break;
    case PARAM1:
      param1_ = ui_[PARAM1].getValue(); // TODO 加工して代入する
      break;
    case PARAM2:
      param2_ = ui_[PARAM2].getValue(); // TODO 加工して代入する
      break;
    }
  }

public:
  /// @brief コンストラクタ
  EffectorTemplate() //
      : ui_({
            EffectParameter<float>(1, 100, 40, 1, "P0"), // 最小値1, 最大値100, 初期値40, ステップ1, 表示文字
            EffectParameter<float>(1, 100, 1, "P1"),     // 最小値1, 最大値100, 初期値省略（最大最小の中間値）, ステップ1, 表示文字
            EffectParameter<float>(1, 100, 60, 1, "P2"), // 最小値1, 最大値100, 初期値60, ステップ1, 表示文字
        }),                                              //
        param0_(0),                                      //
        param1_(0),                                      //
        param2_(0)                                       //
  {
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      update(n);
    }
  }
  /// @brief デストラクタ
  virtual ~EffectorTemplate() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      // TODO エフェクト処理
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
    case PARAM0:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[PARAM0].getValue())));
    case PARAM1:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[PARAM1].getValue())));
    case PARAM2:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[PARAM2].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x20, 0x20, 0x20}; }
};
