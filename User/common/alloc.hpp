/// @file      common/alloc.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cmsis_os.h>
#include <cstddef> // std::size_t
#include <utility> // std::move

namespace satoh
{
template <typename T>
class Alloc;
template <typename T>
class AllocCls;
} // namespace satoh

/// @brief メモリ確保（要素のコンストラクタ・デストラクタは呼ばない版）
/// @tparam T 型
template <typename T>
class satoh::Alloc
{
  /// コピーコンストラクタ削除
  Alloc(Alloc const &) = delete;
  /// 代入演算子削除
  Alloc &operator=(Alloc const &) = delete;

  T *m_;             ///< メモリ
  std::size_t size_; ///< 要素数

public:
  /// @brief コンストラクタ
  Alloc() : m_(0), size_(0) {}
  /// @brief コンストラクタ
  /// @param[in] size 要素数
  explicit Alloc(std::size_t size)                            //
      : m_(static_cast<T *>(pvPortMalloc(size * sizeof(T)))), //
        size_(size)                                           //
  {
  }
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  explicit Alloc(Alloc &&that) //
      : m_(that.m_),           //
        size_(that.size_)      //
  {
    that.m_ = 0;
    that.size_ = 0;
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
      size_ = that.size_;
      that.m_ = 0;
      that.size_ = 0;
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
      size_ = 0;
    }
  }
  /// @brief 要素数取得
  /// @return 要素数
  std::size_t size() const noexcept { return size_; }
  /// @brief 有効なリソースを所有しているかを判定する。
  /// @retval true 所有している
  /// @retval false 所有していない
  explicit operator bool() const noexcept { return !!m_; }
  /// @brief ポインタ取得
  /// @return ポインタ
  T *get() noexcept { return m_; }
  /// @brief ポインタ取得
  /// @return ポインタ
  T const *get() const noexcept { return const_cast<Alloc *>(this)->get(); }
  /// @brief 参照取得
  /// @return 参照
  T &operator*() { return *get(); }
  /// @brief 参照取得
  /// @return 参照
  T const &operator*() const { return *const_cast<Alloc *>(this); }
  /// @brief 任意の位置の要素にアクセスする。
  /// @param[in] i インデックス
  T &operator[](std::size_t i) { return m_[i]; }
  /// @brief 任意の位置の要素にアクセスする。
  /// @param[in] i インデックス
  T const &operator[](std::size_t i) const { return (*const_cast<Alloc *>(this))[i]; }
};

/// @brief メモリ確保（コンストラクタ・デストラクタを呼ぶ版）
/// @tparam T 型
template <typename T>
class satoh::AllocCls
{
  /// コピーコンストラクタ削除
  AllocCls(AllocCls const &) = delete;
  /// 代入演算子削除
  AllocCls &operator=(AllocCls const &) = delete;

  Alloc<T> m_; ///< メモリ
  T *ptr_;     ///< new確保用メモリ

public:
  /// @brief コンストラクタ
  explicit AllocCls() : ptr_(0) {}
  /// @brief コンストラクタ
  /// @tparam Args コンストラクタ引数型
  template <class... Args>
  explicit AllocCls(Args... args) : m_(1), ptr_(new (m_.get()) T(args...))
  {
  }
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  AllocCls(AllocCls &&that) //
      : m_(std::move(that.m_)), ptr_(that.ptr_)
  {
    that.ptr_ = 0;
  }
  /// @brief move演算子
  /// @param[in] that 移動元
  /// @return 自身の参照
  AllocCls &operator=(AllocCls &&that)
  {
    if (this != &that)
    {
      reset();
      m_ = std::move(that.m_);
      ptr_ = that.ptr_;
      that.ptr_ = 0;
    }
    return *this;
  }
  /// @brief デストラクタ
  virtual ~AllocCls() { reset(); }
  /// @brief メモリ開放
  void reset() noexcept
  {
    if (ptr_)
    {
      delete ptr_;
      ptr_ = 0;
      m_.reset();
    }
  }
  /// @brief 有効なリソースを所有しているかを判定する。
  /// @retval true 所有している
  /// @retval false 所有していない
  explicit operator bool() const noexcept { return static_cast<bool>(m_); }
  /// @brief ポインタ取得
  /// @return ポインタ
  T *get() noexcept { return m_.get(); }
  /// @brief ポインタ取得
  /// @return ポインタ
  T const *get() const noexcept { return const_cast<AllocCls *>(this)->get(); }
  /// @brief 参照取得
  /// @return 参照
  T &operator*() { return *m_; }
  /// @brief 参照取得
  /// @return 参照
  T const &operator*() const { return *const_cast<AllocCls *>(this); }
  /// @brief ポインタ取得
  /// @return ポインタ
  T *operator->() noexcept { return get(); }
  /// @brief ポインタ取得
  /// @return ポインタ
  T const *operator->() const noexcept { return get(); }
};
