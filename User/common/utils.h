/// @file      common/rgb.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstddef>

namespace satoh
{
/// @brief 配列要素数を返す
/// @tparam T 配列要素型
/// @tparam N 配列要素数
/// @param [in] a 配列
/// @return 配列要素数
/// @note C++17のstd::sizeと同等
template <typename T, size_t N>
constexpr inline size_t countof(T const (&a)[N])
{
  return N;
}
} // namespace satoh