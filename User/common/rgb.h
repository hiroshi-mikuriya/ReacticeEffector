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
}

/// @brief 色値型
struct satoh::RGB
{
  uint8_t red;   ///< 赤
  uint8_t green; ///< 緑
  uint8_t blue;  ///< 青
};
