/// @file      device/neo_pixel.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "neo_pixel.h"
#include <string.h> // memset

namespace
{
/// @brief NeoPixelに送信する形式で色値をバッファに格納する
/// @param[in] c 色値
/// @param[in] p 格納先
inline void set(uint8_t c, uint8_t *p) noexcept
{
  constexpr uint8_t NEO_0 = 0b11100000;
  constexpr uint8_t NEO_1 = 0b11111000;
  *(p++) = (c & 0x80) ? NEO_1 : NEO_0;
  *(p++) = (c & 0x40) ? NEO_1 : NEO_0;
  *(p++) = (c & 0x20) ? NEO_1 : NEO_0;
  *(p++) = (c & 0x10) ? NEO_1 : NEO_0;
  *(p++) = (c & 0x08) ? NEO_1 : NEO_0;
  *(p++) = (c & 0x04) ? NEO_1 : NEO_0;
  *(p++) = (c & 0x02) ? NEO_1 : NEO_0;
  *(p++) = (c & 0x01) ? NEO_1 : NEO_0;
}
/// @brief LED番号に対応するバッファのインデックスを取得
/// @param[in] ledIndex LED番号
/// @return バッファのインデックス
inline uint32_t getBufferIndex(uint32_t ledNum) noexcept
{
  return ledNum * 3 * 8;
}
} // namespace

satoh::NeoPixel::NeoPixel(SpiMaster *spi, uint32_t ledCount) noexcept //
    : spi_(spi),                                                      //
      ledCount_(ledCount),                                            //
      buf_(new uint8_t[getBufferIndex(ledCount_)])                    //
{
  clear();
}

satoh::NeoPixel::~NeoPixel() {}

void satoh::NeoPixel::set(RGB const &rgb, uint32_t n) noexcept
{
  if (n < ledCount_)
  {
    uint8_t *p = buf_.get() + getBufferIndex(n);
    ::set(rgb.green, p);
    ::set(rgb.red, p + 8);
    ::set(rgb.blue, p + 16);
  }
}
void satoh::NeoPixel::clear() noexcept
{
  memset(buf_.get(), 0, getBufferIndex(ledCount_));
}
bool satoh::NeoPixel::show() const noexcept
{
  return SpiMaster::OK == spi_->send(buf_.get(), getBufferIndex(ledCount_));
}
