/// @file      device/pca9635.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "pca9635.h"
#include <cstring> // memcpy

namespace
{
constexpr uint8_t SLAVE_ADDR = 0x60 << 1;
constexpr uint8_t MODE1 = 0x00;
constexpr uint8_t MODE2 = 0x01;
constexpr uint8_t PWM0 = 0x02;
constexpr uint8_t PWM1 = 0x03;
constexpr uint8_t PWM2 = 0x04;
constexpr uint8_t PWM3 = 0x05;
constexpr uint8_t PWM4 = 0x06;
constexpr uint8_t PWM5 = 0x07;
constexpr uint8_t PWM6 = 0x08;
constexpr uint8_t PWM7 = 0x09;
constexpr uint8_t PWM8 = 0x0a;
constexpr uint8_t PWM9 = 0x0b;
constexpr uint8_t PWM10 = 0x0c;
constexpr uint8_t PWM11 = 0x0d;
constexpr uint8_t PWM12 = 0x0e;
constexpr uint8_t PWM13 = 0x0f;
constexpr uint8_t PWM14 = 0x10;
constexpr uint8_t PWM15 = 0x11;
constexpr uint8_t GRPPWM = 0x12;
constexpr uint8_t GPRFREQ = 0x13;
constexpr uint8_t LEDOUT0 = 0x14;
constexpr uint8_t LEDOUT1 = 0x15;
constexpr uint8_t LEDOUT2 = 0x16;
constexpr uint8_t LEDOUT3 = 0x17;
constexpr uint8_t SUBADR1 = 0x18;
constexpr uint8_t SUBADR2 = 0x19;
constexpr uint8_t SUBADR3 = 0x1a;
constexpr uint8_t ALLCALLADR = 0x1b;
} // namespace

bool satoh::PCA9635::write(uint8_t reg, uint8_t const *bytes, uint32_t size)
{
  uint8_t d[32] = {0};
  d[0] = static_cast<uint8_t>(reg | 0x80);
  memcpy(d + 1, bytes, size);
  return I2C::Result::OK == i2c_->write(SLAVE_ADDR, d, size + 1);
}

satoh::PCA9635::PCA9635(I2C *i2c) noexcept : i2c_(i2c)
{
  uint8_t v0[] = {0b10000001, 0b00000001};
  uint8_t v1[] = {0xFF, 0xFF, 0xFF, 0x00};
  write(MODE1, v0, sizeof(v0)) && write(LEDOUT0, v1, sizeof(v1));
}
satoh::PCA9635::~PCA9635() {}
bool satoh::PCA9635::set(RGB const &rgb, uint8_t n)
{
  if (3 < n)
  {
    return false;
  }
  uint8_t reg = PWM0 + n * 3;
  uint8_t const v[] = {rgb.red, rgb.green, rgb.blue};
  return write(reg, v, sizeof(v));
}