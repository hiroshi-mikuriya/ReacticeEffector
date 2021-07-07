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

inline constexpr satoh::RGB operator+(satoh::RGB const &lh, satoh::RGB const &rh)
{
  return satoh::RGB{
      static_cast<uint8_t>(lh.red + rh.red),     //
      static_cast<uint8_t>(lh.green + rh.green), //
      static_cast<uint8_t>(lh.blue + rh.blue)    //
  };
}
inline satoh::RGB &operator+=(satoh::RGB &lh, satoh::RGB const &rh)
{
  lh.red += rh.red;
  lh.green += rh.green;
  lh.blue += rh.blue;
  return lh;
}
inline constexpr satoh::RGB operator-(satoh::RGB const &lh, satoh::RGB const &rh)
{
  return satoh::RGB{
      static_cast<uint8_t>(lh.red - rh.red),     //
      static_cast<uint8_t>(lh.green - rh.green), //
      static_cast<uint8_t>(lh.blue - rh.blue)    //
  };
}
inline satoh::RGB &operator-=(satoh::RGB &lh, satoh::RGB const &rh)
{
  lh.red -= rh.red;
  lh.green -= rh.green;
  lh.blue -= rh.blue;
  return lh;
}
inline constexpr satoh::RGB satoh::operator*(RGB const &rgb, int a)
{
  return RGB{
      static_cast<uint8_t>(rgb.red * a),   //
      static_cast<uint8_t>(rgb.green * a), //
      static_cast<uint8_t>(rgb.blue * a)   //
  };
}
inline satoh::RGB &satoh::operator*=(RGB &rgb, int a)
{
  rgb.red *= a;
  rgb.green *= a;
  rgb.blue *= a;
  return rgb;
}
inline constexpr satoh::RGB satoh::operator/(RGB const &rgb, int a)
{
  return RGB{
      static_cast<uint8_t>(rgb.red / a),   //
      static_cast<uint8_t>(rgb.green / a), //
      static_cast<uint8_t>(rgb.blue / a)   //
  };
}
inline satoh::RGB &satoh::operator/=(RGB &rgb, int a)
{
  rgb.red /= a;
  rgb.green /= a;
  rgb.blue /= a;
  return rgb;
}
