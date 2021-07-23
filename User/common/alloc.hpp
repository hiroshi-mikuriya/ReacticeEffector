/// @file      common/alloc.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cmsis_os.h>
#include <cstring>

namespace satoh
{
template <typename T>
class Alloc;
}

/// @brief メモリ確保
/// @tparam T 型
/// @note T型のコンストラクタ・デストラクタは呼ばれない
template <typename T>
class satoh::Alloc
{
  /// コピーコンストラクタ削除
  Alloc(Alloc const &) = delete;
  /// コピー演算子削除
  Alloc &operator=(Alloc const &) = delete;

  T *m_;           ///< メモリ
  uint32_t count_; ///< 要素数

public:
  /// @brief コンストラクタ
  Alloc() : m_(0), count_(0) {}
  /// @brief コンストラクタ
  /// @param[in] count 要素数
  explicit Alloc(uint32_t count)                               //
      : m_(static_cast<T *>(pvPortMalloc(count * sizeof(T)))), //
        count_(count)                                          //
  {
    memset(m_, 0, count_ * sizeof(T));
  }
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  explicit Alloc(Alloc &&that) //
      : m_(that.m_),           //
        count_(that.count_)    //
  {
    that.m_ = 0;
    that.count_ = 0;
  }
  /// @brief move演算子
  /// @param[in] that 移動元
  /// @return 自身の参照
  Alloc &operator=(Alloc &&that)
  {
    if (this != &that)
    {
      reset();
      m_ = that.m_;
      count_ = that.count_;
      that.m_ = 0;
      that.count_ = 0;
    }
    return *this;
  }
  /// @brief デストラクタ
  virtual ~Alloc() { reset(); }
  /// @brief メモリ開放
  void reset() noexcept
  {
    if (m_)
    {
      vPortFree(m_);
      m_ = 0;
      count_ = 0;
    }
  }
  /// @brief メモリ確保成功・失敗を取得
  /// @retval true 成功
  /// @retval false 失敗
  bool ok() const noexcept { return !!m_; }
  /// @brief メモリ取得
  /// @return メモリ
  T *get() noexcept { return m_; }
  /// @brief メモリ取得
  /// @return メモリ
  T const *get() const noexcept { return m_; }
  /// @brief 要素数取得
  /// @return 要素数
  uint32_t count() const noexcept { return count_; }
};