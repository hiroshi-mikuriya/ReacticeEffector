/// @file      common/alloc.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cmsis_os.h>
#include <memory>  // std::unique_ptr
#include <utility> // std::move

namespace satoh
{
template <typename T, class... Args>
T *alloc(Args... args) noexcept;

template <typename T>
T *allocArray(std::size_t size) noexcept;

template <typename T>
class Deleter;

template <typename T>
using UniquePtr = std::unique_ptr<T, Deleter<T>>;
} // namespace satoh

/// @brief RTOSからメモリ確保し、コンストラクタを呼び出しポインタを返す
/// @tparam T メモリ確保する型
/// @tparam Args コンストラクタ引数型
/// @param[in] args コンストラクタ引数
template <typename T, class... Args>
T *satoh::alloc(Args... args) noexcept
{
  void *ptr = pvPortMalloc(sizeof(T));
  return new (ptr) T(args...);
}

/// @brief RTOSからメモリ確保し、コンストラクタを呼ばずにポインタを返す
/// @tparam T メモリ確保する型
/// @param[in] size メモリ確保する数
template <typename T>
T *satoh::allocArray(std::size_t size) noexcept
{
  return static_cast<T *>(pvPortMalloc(size * sizeof(T)));
}

/// @brief satoh::allocでメモリ確保したオブジェクトを破棄する
template <typename T = void>
class satoh::Deleter
{
public:
  constexpr Deleter() noexcept = default;

  // 別の Deleter オブジェクトから Deleter オブジェクトを構築します。
  // このコンストラクタは U* が T* に暗黙に変換可能な場合にのみ、オーバーロード解決に参加します。
  template <typename U, typename std::enable_if<std::is_convertible<U *, T *>::value, std::nullptr_t>::type = nullptr>
  explicit Deleter(const Deleter<U> &) noexcept
  {
  }
  /// @brief オブジェクト破棄
  /// @param[in] ptr オブジェクトのポインタ
  void operator()(T *ptr) const
  {
    ptr->~T(); // 配置newしているので意図的にデストラクタを呼び出す必要がある
    vPortFree(ptr);
  }
};

/// @brief satoh::allocArrayでメモリ確保したオブジェクトを破棄する
template <typename T>
class satoh::Deleter<T[]>
{
public:
  constexpr Deleter() noexcept = default;

  // 別の Deleter オブジェクトから Deleter オブジェクトを構築します。
  // このコンストラクタは U(*)[] が T(*)[] に暗黙に変換可能な場合にのみ、オーバーロード解決に参加します。
  template <typename U, typename std::enable_if<std::is_convertible<U (*)[], T (*)[]>::value, std::nullptr_t>::type = nullptr>
  Deleter(const Deleter<U[]> &) noexcept
  {
  }
  /// @brief オブジェクト破棄
  /// @param[in] ptr オブジェクトのポインタ
  void operator()(T *ptr) const { vPortFree(ptr); }
};
