/// @file      common/rgb.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
struct RGB;

constexpr RGB operator+(RGB const &lh, RGB const &rh);
RGB &operator+=(RGB &lh, RGB const &rh);
constexpr RGB operator-(RGB const &lh, RGB const &rh);
RGB &operator-=(RGB &lh, RGB const &rh);
constexpr RGB operator*(RGB const &rgb, int a);
RGB &operator*=(RGB &rgb, int a);
constexpr RGB operator/(RGB const &rgb, int a);
RGB &operator/=(RGB &rgb, int a);
} // namespace satoh

/// @brief 色値型
struct satoh::RGB
{
  uint8_t red;   ///< 赤
  uint8_t green; ///< 緑
  uint8_t blue;  ///< 青
};

/// @brief 加算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline constexpr satoh::RGB satoh::operator+(satoh::RGB const &lh, satoh::RGB const &rh)
{
  return satoh::RGB{
      static_cast<uint8_t>(lh.red + rh.red),     //
      static_cast<uint8_t>(lh.green + rh.green), //
      static_cast<uint8_t>(lh.blue + rh.blue)    //
  };
}
/// @brief 加算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline satoh::RGB &satoh::operator+=(satoh::RGB &lh, satoh::RGB const &rh)
{
  return lh = lh + rh;
}
/// @brief 減算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline constexpr satoh::RGB satoh::operator-(satoh::RGB const &lh, satoh::RGB const &rh)
{
  return satoh::RGB{
      static_cast<uint8_t>(lh.red - rh.red),     //
      static_cast<uint8_t>(lh.green - rh.green), //
      static_cast<uint8_t>(lh.blue - rh.blue)    //
  };
}
/// @brief 減算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline satoh::RGB &satoh::operator-=(satoh::RGB &lh, satoh::RGB const &rh)
{
  return lh = lh - rh;
}
/// @brief 乗算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline constexpr satoh::RGB satoh::operator*(RGB const &rgb, int a)
{
  return RGB{
      static_cast<uint8_t>(rgb.red * a),   //
      static_cast<uint8_t>(rgb.green * a), //
      static_cast<uint8_t>(rgb.blue * a)   //
  };
}
/// @brief 乗算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline satoh::RGB &satoh::operator*=(RGB &rgb, int a)
{
  return rgb = rgb * a;
}
/// @brief 除算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline constexpr satoh::RGB satoh::operator/(RGB const &rgb, int a)
{
  return RGB{
      static_cast<uint8_t>(rgb.red / a),   //
      static_cast<uint8_t>(rgb.green / a), //
      static_cast<uint8_t>(rgb.blue / a)   //
  };
}
/// @brief 除算演算子 @param[in] lh 左辺値 @param[in] rh 右辺値 @return 演算結果
inline satoh::RGB &satoh::operator/=(RGB &rgb, int a)
{
  return rgb = rgb / a;
}
