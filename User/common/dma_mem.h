/// @file      common/dma_mem.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstddef> // std::size_t
#include <memory>

namespace satoh
{
/// @brief DMA転送に使うメモリを確保する
/// @param [in] size 確保するバイト数
/// @retval 0以外 確保したメモリのポインタ
/// @retval 0 確保失敗（容量不足）
void *allocDmaMem(std::size_t size) noexcept;
/// @brief DMA転送に使うメモリを開放する
/// @param [in] ptr 開放するメモリ
/// @note 現在は開放できない
void freeDmaMem(void *ptr) noexcept;
/// @brief DMA転送に使うメモリの残量を取得する
/// @return DMA転送に使うメモリの残量
size_t getFreeDmaMemSize() noexcept;
/// @brief DMA転送メモリを開放するクラス
/// @tparam T データ型
template <typename T>
class DmaMemDeleter;
/// @brief DMA転送メモリを管理するユニークポインタ型
/// @tparam T データ型
template <typename T>
using UniqueDmaPtr = std::unique_ptr<T, DmaMemDeleter<T>>;
/// @brief DMA転送メモリを管理するユニークポインタを作る
/// @tparam T データ型
/// @param [in] size 要素数
template <typename T>
UniqueDmaPtr<T> makeDmaMem(std::size_t size) noexcept;
} // namespace satoh

/// @brief satoh::allocDmaMemで確保したメモリを破棄する
template <typename T = void>
class satoh::DmaMemDeleter
{
public:
  constexpr DmaMemDeleter() noexcept = default;

  // 別の DmaMemDeleter オブジェクトから DmaMemDeleter オブジェクトを構築します。
  // このコンストラクタは U* が T* に暗黙に変換可能な場合にのみ、オーバーロード解決に参加します。
  template <typename U, typename std::enable_if<std::is_convertible<U *, T *>::value, std::nullptr_t>::type = nullptr>
  explicit DmaMemDeleter(const DmaMemDeleter<U> &) noexcept
  {
  }
  /// @brief オブジェクト破棄
  /// @param [in] ptr オブジェクトのポインタ
  void operator()(T *ptr) const { freeDmaMem(ptr); }
};

template <typename T>
satoh::UniqueDmaPtr<T> satoh::makeDmaMem(std::size_t size) noexcept
{
  return satoh::UniqueDmaPtr<T>(static_cast<T *>(allocDmaMem(size * sizeof(T))));
}
