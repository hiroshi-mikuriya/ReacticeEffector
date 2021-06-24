/// @file      neo_pixel.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "neo_pixel.h"
#include "main.h"
#include <string.h> // memset

namespace
{
/// @brief NeoPixelのPWMデータへ変換
/// @param[in] src 色データ
/// @param[in] count LED数
/// @param[out] buffer PWMデータ
void convert(uint8_t const *rgb, uint32_t count, uint8_t *buffer)
{
  constexpr uint8_t NEO_0 = 0b11100000;
  constexpr uint8_t NEO_1 = 0b11111000;
  uint8_t *p = buffer;
  for (uint32_t i = 0; i < count; ++i)
  {
    for (uint32_t j = 0; j < 3; ++j)
    {
      const uint8_t c = rgb[j];
      *(p++) = (c & 0x80) ? NEO_1 : NEO_0;
      *(p++) = (c & 0x40) ? NEO_1 : NEO_0;
      *(p++) = (c & 0x20) ? NEO_1 : NEO_0;
      *(p++) = (c & 0x10) ? NEO_1 : NEO_0;
      *(p++) = (c & 0x08) ? NEO_1 : NEO_0;
      *(p++) = (c & 0x04) ? NEO_1 : NEO_0;
      *(p++) = (c & 0x02) ? NEO_1 : NEO_0;
      *(p++) = (c & 0x01) ? NEO_1 : NEO_0;
    }
  }
}
} // namespace

sato::NeoPixel::NeoPixel() noexcept
{
  memset(buf_, 0, sizeof(buf_));
  LL_SPI_Enable(SPI3);
  const uint8_t c0[3] = {0x00 /*green*/, 0x10 /*red*/, 0x10 /*blue*/};
  const uint8_t c1[3] = {0x10 /*green*/, 0x10 /*red*/, 0x00 /*blue*/};
  const uint8_t c2[3] = {0x10 /*green*/, 0x00 /*red*/, 0x10 /*blue*/};
  uint8_t buffer[(N * 3) * 8] = {0};
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_5, (uint32_t)&buffer, LL_SPI_DMA_GetRegAddr(SPI3), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_5);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_5, sizeof(buffer));
  LL_SPI_EnableDMAReq_TX(SPI3);
}

sato::NeoPixel::~NeoPixel()
{
  LL_SPI_Disable(SPI3);
}