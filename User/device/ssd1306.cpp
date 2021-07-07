/// @file      device/ssd1306.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "ssd1306.h"
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

namespace
{
constexpr uint8_t DotB1[8] = {0b11111111, 0b00000111, 0b11111111, 0b00000011, 0b00000101, 0b00001001, 0b00010001, 0b00100001};
}

void satoh::SSD1306::updateScreen()
{
  for (uint8_t page = 0; page < PAGE; ++page)
  {
    uint8_t addr = static_cast<uint8_t>(0xB0 | page);
    uint8_t v0[] = {
        CTRL_10,   // control byte, Co bit = 1, D/C# = 0 (command)
        addr,      // set page start address(B0～B7)
        CTRL_00,   // control byte, Co bit = 0, D/C# = 0 (command)
        0x21,      // set Column Address
        0,         // Column Start Address(0-127)
        WIDTH - 1, // Column Stop Address(0-127)
    };
    write(v0, sizeof(v0));
    // column = 8byte x 16
    for (int col = 0; col < 16; ++col)
    {
      uint8_t v1[9] = {
          CTRL_01, //control byte, Co bit = 0 (continue), D/C# = 1 (data)
      };
      write(v1, sizeof(v1));
    }
  }
  // とりあえず図形を表示
  {
    uint8_t v[] = {
        CTRL_10,   // control byte, Co bit = 1, D/C# = 0 (command)
        0xB0,      // set page start address←垂直開始ページはここで決める(B0～B7)
        CTRL_00,   // control byte, Co bit = 0, D/C# = 0 (command)
        0x21,      // set Column Address
        0,         // Column Start Address←水平開始位置はここで決める(0～127)
        WIDTH - 1, // Column Stop Address　画面をフルに使う
    };
    write(v, sizeof(v));
  }
  {
    uint8_t v[] = {
        CTRL_01,  // control byte, Co bit = 0 (continue), D/C# = 1 (data)
        DotB1[0], //
        DotB1[1], //
        DotB1[2], //
        DotB1[3], //
        DotB1[4], //
        DotB1[5], //
        DotB1[6], //
        DotB1[7], //
    };
    write(v, sizeof(v));
  }
}

satoh::SSD1306::SSD1306(I2C *i2c) noexcept //
    : I2CDeviceBase(i2c, SLAVE_ADDR),      //
      buf_(new uint8_t[BUF_SIZE]),         //
      ok_(init())                          //
{
  if (ok_)
  {
    memset(buf_.get(), 0, BUF_SIZE);
    updateScreen();
  }
}

satoh::SSD1306::~SSD1306() {}

bool satoh::SSD1306::ok() const noexcept
{
  return ok_;
}