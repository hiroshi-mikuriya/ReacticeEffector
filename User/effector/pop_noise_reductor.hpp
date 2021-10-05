/// @file      effector/pop_noise_reductor.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <algorithm>
#include <cstdint>

namespace satoh
{
namespace fx
{
class PopNoiseReductor;
}
} // namespace satoh

/// @brief ポップノイズ除去クラス
class satoh::fx::PopNoiseReductor
{
  uint32_t count_; ///< 音声切替直後のポップノイズ除去対象音声数
  uint32_t index_; ///< インデックス

public:
  /// @brief コンストラクタ @param[in] count 音声切替直後のポップノイズ除去対象音声数
  explicit PopNoiseReductor(uint32_t count) noexcept : count_(std::max<uint32_t>(1, count)), index_(0) { init(); }
  /// @brief デストラクタ
  virtual ~PopNoiseReductor() noexcept {}
  /// @brief 初期化
  void init() noexcept { index_ = 0; }
  /// @brief ポップノイズ除去処理
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param [in] size 音声データ数
  void reduct(float *left, float *right, uint32_t size) noexcept
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      if (count_ <= index_)
      {
        return;
      }
      float a = 1.0f * index_ / count_;
      left[i] *= a;
      right[i] *= a;
      ++index_;
    }
  }
};
