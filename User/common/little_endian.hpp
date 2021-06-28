/// @file      common/little_endian.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
/// @brief リトルエンディアン変換クラステンプレート
/// @tparam T 変換する型
template <typename T>
class LE
{
};
/// @brief リトルエンディアン変換クラス（bool版）
template <>
class LE<bool>
{
public:
  static bool get(uint8_t const *src) { return !!src[0]; }
  static void set(bool src, uint8_t *dst) { dst[0] = src ? 1 : 0; }
};
/// @brief リトルエンディアン変換クラス（uint8_t版）
template <>
class LE<uint8_t>
{
public:
  static uint8_t get(uint8_t const *src) { return src[0]; }
  static void set(uint8_t src, uint8_t *dst) { dst[0] = src; }
};
/// @brief リトルエンディアン変換クラス（int8_t版）
template <>
class LE<int8_t>
{
public:
  static int8_t get(uint8_t const *src) { return static_cast<int8_t>(LE<uint8_t>::get(src)); }
  static void set(int8_t src, uint8_t *dst) { LE<uint8_t>::set(static_cast<uint8_t>(src), dst); }
};
/// @brief リトルエンディアン変換クラス（uint16_t版）
template <>
class LE<uint16_t>
{
public:
  static uint16_t get(uint8_t const *src)
  {                                                                 //
    return static_cast<uint16_t>((LE<uint8_t>::get(src + 1) << 8) + //
                                 LE<uint8_t>::get(src));
  }
  static void set(uint16_t src, uint8_t *dst)
  {
    dst[0] = static_cast<uint8_t>(src);
    dst[1] = static_cast<uint8_t>(src >> 8);
  }
};
/// @brief リトルエンディアン変換クラス（int16_t版）
template <>
class LE<int16_t>
{
public:
  static int16_t get(uint8_t const *src) { return static_cast<int16_t>(LE<uint16_t>::get(src)); }
  static void set(int16_t src, uint8_t *dst) { LE<uint16_t>::set(static_cast<uint16_t>(src), dst); }
};
/// @brief リトルエンディアン変換クラス（uint32_t版）
template <>
class LE<uint32_t>
{
public:
  static uint32_t get(uint8_t const *src)
  {                                                                   //
    return static_cast<uint32_t>((LE<uint16_t>::get(src + 2) << 16) + //
                                 LE<uint16_t>::get(src));
  }
  static void set(uint32_t src, uint8_t *dst)
  {
    LE<uint16_t>::set(static_cast<uint16_t>(src), dst);
    LE<uint16_t>::set(static_cast<uint16_t>(src >> 16), dst + 2);
  }
};
/// @brief リトルエンディアン変換クラス（int32_t版）
template <>
class LE<int32_t>
{
public:
  static int32_t get(uint8_t const *src) { return static_cast<int32_t>(LE<uint32_t>::get(src)); }
  static void set(int32_t src, uint8_t *dst) { LE<uint32_t>::set(static_cast<uint32_t>(src), dst); }
};
} // namespace satoh
