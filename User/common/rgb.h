/// @file      common/rgb.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include <cstdint>

namespace satoh
{
struct RGB;
}

/// @brief 色値型
struct satoh::RGB
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};
