/// @file      effector/effector_base.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include <cstdint>
#include <cstring> // strlen, memcpy

namespace satoh
{
class EffectorBase;
template <typename T>
class EffectParameter;

/// @brief データ圧縮
/// @param[in] min 最小値
/// @param[inout] v 圧縮対象データ
/// @param[in] max 最大値
inline void compress(float min, float &v, float max) noexcept
{
  if (v < min)
  {
    v = min;
  }
  if (max < v)
  {
    v = max;
  }
}
} // namespace satoh

/// @brief エフェクター基底クラス
class satoh::EffectorBase
{
public:
  /// @brief デストラクタ
  virtual ~EffectorBase() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  virtual void effect(float *left, float *right, uint32_t size) noexcept = 0;
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  virtual uint32_t getName(char *buf) const noexcept = 0;
  /// @brief パラメータ数を取得
  /// @return パラメータ数
  virtual uint32_t getParamCount() const noexcept = 0;
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  virtual void incrementParam(uint32_t n) noexcept = 0;
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  virtual void decrementParam(uint32_t n) noexcept = 0;
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] ratio 比率（0.0f 〜 1.0f）
  virtual void setParam(uint32_t n, float ratio) noexcept = 0;
  /// @brief パラメータ名文字列取得
  /// @param[in] n パラメータ番号
  /// @param[out] buf 文字列格納先
  /// @return 文字列の長さ
  virtual uint32_t getParamName(uint32_t n, char *buf) const noexcept = 0;
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @param[out] buf 文字列格納先
  /// @return 文字列の長さ
  virtual uint32_t getValueTxt(uint32_t n, char *buf) const noexcept = 0;
  /// @brief LED色を取得
  /// @return LED色
  virtual RGB getColor() const noexcept = 0;
};

/// @brief エフェクトパラメータが共通で持つ属性
/// @tparam T パラメータの型
template <typename T>
class satoh::EffectParameter
{
  const T min_;               ///< 最小値
  const T max_;               ///< 最大値
  T v_;                       ///< 値
  const T step_;              ///< 目盛り
  char name_[8];              ///< パラメータ名
  const uint32_t nameLength_; ///< パラメータ文字列長

  /// @brief データ圧縮
  void compress() noexcept { satoh::compress(min_, v_, max_); }

public:
  /// @brief コンストラクタ
  /// @param[in] min 最小値
  /// @param[in] max 最大値
  /// @param[in] v 初期値
  /// @param[in] step 目盛り
  /// @param[in] name パラメータ名
  explicit EffectParameter(T min, T max, T v, T step, const char *name) noexcept //
      : min_(min), max_(max), v_(v), step_(step), nameLength_(strlen(name))
  {
    strcpy(name_, name);
  }
  /// @brief コンストラクタ（初期値は最大と最小の中間値にする）
  /// @param[in] min 最小値
  /// @param[in] max 最大値
  /// @param[in] step 目盛り
  /// @param[in] name パラメータ名
  explicit EffectParameter(T min, T max, T step, const char *name) noexcept //
      : EffectParameter(min, max, (max + min) / 2, step, name)
  {
  }
  /// @brief 値取得
  /// @return 値
  T getValue() const noexcept { return v_; }
  /// @brief 最大値取得
  /// @return 最大値
  T getMax() const noexcept { return max_; }
  /// @brief 最小値取得
  /// @return 最小値
  T getMin() const noexcept { return min_; }
  /// @brief 目盛り取得
  /// @return 目盛り
  T getStep() const noexcept { return step_; }
  /// @brief パラメータ名取得
  /// @param[out] buf 格納先
  /// @return 文字列長
  uint32_t getName(char *buf) const noexcept
  {
    strcpy(buf, name_);
    return nameLength_;
  }
  /// @brief 加算する
  void increment() noexcept
  {
    v_ += step_;
    compress();
  }
  /// @brief 減算する
  void decrement() noexcept
  {
    v_ -= step_;
    compress();
  }
  /// @brief 値を設定する
  /// @param[in] ratio 比率（0.0f 〜 1.0f）
  void setValue(float ratio) noexcept
  {
    v_ = (max_ - min_) * ratio + min_;
    compress();
  }
};
