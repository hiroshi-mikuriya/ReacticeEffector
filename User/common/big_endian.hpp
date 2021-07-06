/// @file      common/big_endian.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
namespace impl
{
/// @brief Nバイトコピー
/// @tparam N コピーバイト数
/// @param[in] src コピー元
/// @param[out] dst コピー先
template <uint32_t N>
void rcopy(uint8_t const *src, uint8_t *dst);
/// @brief 2バイトコピー
/// @param[in] src コピー元
/// @param[out] dst コピー先
template <>
inline void rcopy<2>(uint8_t const *src, uint8_t *dst)
{
  dst[0] = src[1];
  dst[1] = src[0];
}
/// @brief 4バイトコピー
/// @param[in] src コピー元
/// @param[out] dst コピー先
template <>
inline void rcopy<4>(uint8_t const *src, uint8_t *dst)
{
  rcopy<2>(&src[0], &dst[2]);
  rcopy<2>(&src[2], &dst[0]);
}
/// @brief 8バイトコピー
/// @param[in] src コピー元
/// @param[out] dst コピー先
template <>
inline void rcopy<8>(uint8_t const *src, uint8_t *dst)
{
  rcopy<4>(&src[0], &dst[4]);
  rcopy<4>(&src[4], &dst[0]);
}
/// @brief 16バイトコピー
/// @param[in] src コピー元
/// @param[out] dst コピー先
template <>
inline void rcopy<16>(uint8_t const *src, uint8_t *dst)
{
  rcopy<8>(&src[0], &dst[8]);
  rcopy<8>(&src[8], &dst[0]);
}
/// @tparam T 読み書きする型
template <typename T>
class BE
{
  /// @brief 型とバイナリ列を変換する共用体
  union U
  {
    T v;                  ///< 型
    uint8_t a[sizeof(v)]; ///< バイナリ列
  };

public:
  /// @brief バイト列から値を読み出す
  /// @param[in] bytes バイト列
  /// @return 値
  static T get(uint8_t const *bytes)
  {
    U u;
    rcopy<sizeof(u)>(bytes, u.a);
    return u.v;
  }
  /// @brief バッファへ値を書き込む
  /// @param[in] buffer バッファ
  /// @param[in] v 値
  static void set(uint8_t *buffer, T v)
  {
    U u;
    u.v = v;
    rcopy<sizeof(u)>(u.a, buffer);
  }
};
} // namespace impl

/// @brief ビッグエンディアン変換クラスのテンプレート
template <typename T>
class BE;
/// @brief ビッグエンディアン変換クラス（bool版）
template <>
class BE<bool>
{
public:
  static bool get(uint8_t const *src) { return !!src[0]; }
  static void set(uint8_t *dst, bool src) { dst[0] = src ? 1 : 0; }
};
/// @brief ビッグエンディアン変換クラス（uint8_t版）
template <>
class BE<uint8_t>
{
public:
  static uint8_t get(uint8_t const *src) { return src[0]; }
  static void set(uint8_t *dst, uint8_t src) { dst[0] = src; }
};
/// @brief ビッグエンディアン変換クラス（int8_t版）
template <>
class BE<int8_t>
{
public:
  static int8_t get(uint8_t const *src) { return static_cast<int8_t>(BE<uint8_t>::get(src)); }
  static void set(uint8_t *dst, int8_t src) { BE<uint8_t>::set(dst, static_cast<uint8_t>(src)); }
};
/// @brief ビッグエンディアン変換クラス（uint16_t版）
template <>
class BE<uint16_t> : public impl::BE<uint16_t>
{
};
/// @brief ビッグエンディアン変換クラス（int16_t版）
template <>
class BE<int16_t> : public impl::BE<int16_t>
{
};
/// @brief ビッグエンディアン変換クラス（uint32_t版）
template <>
class BE<uint32_t> : public impl::BE<uint32_t>
{
};
/// @brief ビッグエンディアン変換クラス（int32_t版）
template <>
class BE<int32_t> : public impl::BE<int32_t>
{
};
/// @brief ビッグエンディアン変換クラス（uint64_t版）
template <>
class BE<uint64_t> : public impl::BE<uint64_t>
{
};
/// @brief ビッグエンディアン変換クラス（int64_t版）
template <>
class BE<int64_t> : public impl::BE<int64_t>
{
};
/// @brief ビッグエンディアン変換クラス（float版）
template <>
class BE<float> : public impl::BE<float>
{
};
/// @brief ビッグエンディアン変換クラス（double版）
template <>
class BE<double> : public impl::BE<double>
{
};
/// @brief ビッグエンディアン変換クラス（long double版）
template <>
class BE<long double> : public impl::BE<long double>
{
};
} // namespace satoh
