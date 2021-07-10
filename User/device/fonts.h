/// @file      device/fonts.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
struct FontDef
{
  uint8_t width;        /* Font width in pixels */
  uint8_t height;       /* Font height in pixels */
  uint16_t const *data; /* Pointer to data font data array */
};
extern const FontDef Font_7x10;
extern const FontDef Font_11x18;
extern const FontDef Font_16x26;
} // namespace satoh
