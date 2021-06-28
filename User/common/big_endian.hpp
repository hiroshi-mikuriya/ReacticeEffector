/// @file      common/big_endian.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
/// @brief ビッグエンディアン変換クラステンプレート
/// @tparam T 変換する型
template <typename T>
class BE
{
};
/// @brief ビッグエンディアン変換クラス（bool版）
template <>
class BE<bool>
{
public:
  static bool get(uint8_t const *src) { return !!src[0]; }
  static void set(bool src, uint8_t *dst) { dst[0] = src ? 1 : 0; }
};
/// @brief ビッグエンディアン変換クラス（uint8_t版）
template <>
class BE<uint8_t>
{
public:
  static uint8_t get(uint8_t const *src) { return src[0]; }
  static void set(uint8_t src, uint8_t *dst) { dst[0] = src; }
};
/// @brief ビッグエンディアン変換クラス（int8_t版）
template <>
class BE<int8_t>
{
public:
  static int8_t get(uint8_t const *src) { return static_cast<int8_t>(BE<uint8_t>::get(src)); }
  static void set(int8_t src, uint8_t *dst) { BE<uint8_t>::set(static_cast<uint8_t>(src), dst); }
};
/// @brief ビッグエンディアン変換クラス（uint16_t版）
template <>
class BE<uint16_t>
{
public:
  static uint16_t get(uint8_t const *src)
  {                                                             //
    return static_cast<uint16_t>((BE<uint8_t>::get(src) << 8) + //
                                 BE<uint8_t>::get(src + 1));
  }
  static void set(uint16_t src, uint8_t *dst)
  {
    dst[0] = static_cast<uint8_t>(src >> 8);
    dst[1] = static_cast<uint8_t>(src);
  }
};
/// @brief ビッグエンディアン変換クラス（int16_t版）
template <>
class BE<int16_t>
{
public:
  static int16_t get(uint8_t const *src) { return static_cast<int16_t>(BE<uint16_t>::get(src)); }
  static void set(int16_t src, uint8_t *dst) { BE<uint16_t>::set(static_cast<uint16_t>(src), dst); }
};
/// @brief ビッグエンディアン変換クラス（uint32_t版）
template <>
class BE<uint32_t>
{
public:
  static uint32_t get(uint8_t const *src)
  {                                                               //
    return static_cast<uint32_t>((BE<uint16_t>::get(src) << 16) + //
                                 BE<uint16_t>::get(src + 2));
  }
  static void set(uint32_t src, uint8_t *dst)
  {
    BE<uint16_t>::set(static_cast<uint16_t>(src >> 16), dst);
    BE<uint16_t>::set(static_cast<uint16_t>(src), dst + 2);
  }
};
/// @brief ビッグエンディアン変換クラス（int32_t版）
template <>
class BE<int32_t>
{
public:
  static int32_t get(uint8_t const *src) { return static_cast<int32_t>(BE<uint32_t>::get(src)); }
  static void set(int32_t src, uint8_t *dst) { BE<uint32_t>::set(static_cast<uint32_t>(src), dst); }
};
} // namespace satoh
