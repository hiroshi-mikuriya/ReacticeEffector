/// @file      device/ssd1306.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "ssd1306.h"
#include "fonts.h"
#include <cstring> // memset

namespace
{
constexpr uint8_t SLAVE_ADDR = 0x3C;
constexpr uint8_t CTRL_00 = 0b00000000; // control byte, Co bit = 0, D/C# = 0 (command)
constexpr uint8_t CTRL_01 = 0b01000000; //control byte, Co bit = 0 (continue), D/C# = 1 (data)
constexpr uint8_t CTRL_10 = 0b10000000; // control byte, Co bit = 1, D/C# = 0 (command)
constexpr uint8_t WIDTH = 128;
constexpr uint8_t HEIGHT = 64;
constexpr uint8_t PAGE = 8;
constexpr uint32_t BUF_SIZE = WIDTH * HEIGHT / 8;
/// @brief １ピクセル書き込む
/// @param[in] color 色
/// @param[in] x X位置
/// @param[in] y Y位置
/// @param[out] dst 書き込み先
void drawPixel(uint8_t color, uint8_t x, uint8_t y, uint8_t *dst) noexcept
{
  if (x < WIDTH && y < HEIGHT)
  {
    uint8_t xx = WIDTH - x - 1;
    uint8_t yy = HEIGHT - y - 1;
    if (color)
    {
      dst[xx + (yy / 8) * WIDTH] |= 1 << (yy % 8);
    }
    else
    {
      dst[xx + (yy / 8) * WIDTH] &= ~(1 << (yy % 8));
    }
  }
}
/// @brief １文字書き込む
/// @param[in] ch １文字
/// @param[in] font フォントデータ
/// @param[in] invert 反転有無
/// @param[in] x X位置
/// @param[in] y Y位置
/// @param[out] dst 書き込み先
void drawChar(char ch, satoh::FontDef const &font, bool invert, uint8_t x, uint8_t y, uint8_t *dst) noexcept
{
  for (uint8_t dy = 0; dy < font.height; ++dy)
  {
    uint16_t b = font.data[(ch - 32) * font.height + dy];
    for (uint8_t dx = 0; dx < font.width; ++dx)
    {
      uint8_t color = (b << dx) & 0x8000 ? 1 : 0;
      if (invert)
      {
        color = !color;
      }
      drawPixel(color, x + dx, y + dy, dst);
    }
  }
}
/// @brief 文字列を書き込む
/// @param[in] str 文字列
/// @param[in] font フォントデータ
/// @param[in] invert 反転有無
/// @param[in] x X位置
/// @param[in] y Y位置
/// @param[out] dst 書き込み先
void drawString(char const *str, satoh::FontDef const &font, bool invert, uint8_t x, uint8_t y, uint8_t *dst) noexcept
{
  for (char const *p = str; *p; ++p)
  {
    drawChar(*p, font, invert, x, y, dst);
    x += font.width;
  }
}
} // namespace

bool satoh::SSD1306::init() const noexcept
{
  uint8_t v[] = {
      CTRL_10,    // control byte, Co bit = 1, D/C# = 0 (command)
      0xAE,       // display off
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0xA8,       // Set Multiplex Ratio  0xA8, 0x3F
      HEIGHT - 1, // 64MUX
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0xD3,       // Set Display Offset 0xD3, 0x00
      0x00,       //
      CTRL_10,    // control byte, Co bit = 1, D/C# = 0 (command)
      0x40,       // Set Display Start Line 0x40
      CTRL_10,    // control byte, Co bit = 1, D/C# = 0 (command)
      0xA0,       // Set Segment re-map 0xA0/0xA1
      CTRL_10,    // control byte, Co bit = 1, D/C# = 0 (command)
      0xC0,       // Set COM Output Scan Direction 0xC0,/0xC8
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0xDA,       // Set COM Pins hardware configuration 0xDA, 0x02
      0x12,       //
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0x81,       // Set Contrast Control 0x81, default=0x7F
      255,        // 0-255
      CTRL_10,    // control byte, Co bit = 1, D/C# = 0 (command)
      0xA4,       // Disable Entire Display On
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0xA6,       // Set Normal Display 0xA6, Inverse display 0xA7
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0xD5,       // Set Display Clock Divide Ratio/Oscillator Frequency 0xD5, 0x80
      0x80,       //
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0x20,       // Set Memory Addressing Mode
      0x10,       // Page addressing mode
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0x22,       // Set Page Address
      0,          // Start page set
      PAGE - 1,   // End page set
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0x21,       // set Column Address
      0,          // Column Start Address
      WIDTH - 1,  // Column Stop Address
      CTRL_00,    // control byte, Co bit = 0, D/C# = 0 (command)
      0x8D,       // Set Enable charge pump regulator 0x8D, 0x14
      0x14,       //
      CTRL_10,    // control byte, Co bit = 1, D/C# = 0 (command)
      0xAF,       // Display On 0xAF
  };
  return write(v, sizeof(v));
}

bool satoh::SSD1306::sendBufferToDevice(uint8_t page) noexcept
{
  uint8_t addr = static_cast<uint8_t>(0xB0 | page);
  uint8_t v[] = {
      CTRL_10,   // control byte, Co bit = 1, D/C# = 0 (command)
      addr,      // set page start address(B0～B7)
      CTRL_00,   // control byte, Co bit = 0, D/C# = 0 (command)
      0x21,      // set Column Address
      0,         // Column Start Address(0-127)
      WIDTH - 1, // Column Stop Address(0-127)
  };
  write(v, sizeof(v), false);
  uint8_t *tx = txbuf_.get();
  tx[0] = CTRL_01;
  memcpy(&tx[1], dispbuf_.get() + page * WIDTH, WIDTH);
  return write(tx, WIDTH + 1, true);
}

bool satoh::SSD1306::sendBufferToDevice() noexcept
{
  for (uint8_t page = 0; page < PAGE; ++page)
  {
    sendBufferToDevice(page);
  }
  return true;
}

void satoh::SSD1306::drawEffectPage() noexcept
{
  uint8_t *disp = dispbuf_.get();
  memset(disp, 0, BUF_SIZE);
  satoh::FontDef const &titleFont = satoh::Font_11x18;
  satoh::FontDef const &paramFont = satoh::Font_7x10;
  if (!effector_)
  {
    drawString("Bypass", titleFont, false, 0, 0, disp);
    return;
  }
  char txt[16] = {0};
  effector_->getName(txt);
  drawString(txt, titleFont, false, 0, 0, disp);
  uint32_t n = 0;
  for (uint8_t col = 0; col < 2; ++col)
  {
    for (uint8_t row = 0; row < 3; ++row, ++n)
    {
      if (n < effector_->getParamCount())
      {
        uint8_t y = titleFont.height + 6 + row * (paramFont.height + 5);
        uint8_t cx = col * WIDTH / 2;
        effector_->getParamName(n, txt);
        drawString(txt, paramFont, n == selectedParam_, cx, y, disp);
        uint32_t len = effector_->getValueTxt(n, txt);
        uint8_t px = cx + 43;
        if (2 < len)
        {
          px -= paramFont.width * (len - 2); // 3ケタ以上の数値を左にずらして全体が表示されるようにする
        }
        drawString(txt, paramFont, false, px, y, disp);
      }
    }
  }
}

satoh::SSD1306::SSD1306(I2C *i2c) noexcept //
    : I2CDeviceBase(i2c, SLAVE_ADDR),      //
      dispbuf_(new uint8_t[BUF_SIZE]),     //
      txbuf_(new uint8_t[WIDTH + 32]),     //
      ok_(init()),                         //
      effector_(0),                        //
      selectedParam_(0)                    //
{
  if (ok_)
  {
    drawEffectPage();
    sendBufferToDevice();
  }
}

satoh::SSD1306::~SSD1306() {}

bool satoh::SSD1306::ok() const noexcept
{
  return ok_;
}

bool satoh::SSD1306::setEffector(EffectorBase const *effector) noexcept
{
  effector_ = effector;
  selectedParam_ = 0;
  drawEffectPage();
  return sendBufferToDevice();
}
bool satoh::SSD1306::setParamCursor(uint32_t n) noexcept
{
  selectedParam_ = n;
  drawEffectPage();
  return sendBufferToDevice();
}
bool satoh::SSD1306::updateParam() noexcept
{
  drawEffectPage();
  switch (selectedParam_ % 3)
  {
  case 0:
    return sendBufferToDevice(4) && sendBufferToDevice(5);
  case 1:
    return sendBufferToDevice(2) && sendBufferToDevice(3);
  case 2:
    return sendBufferToDevice(0) && sendBufferToDevice(1);
  default:
    return false;
  }
}
