/// @file      device/pca9555.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "level_meter.h"
#include "pca9555.h"

satoh::LevelMeter::LevelMeter(I2C *i2c) noexcept //
    : i2c_(i2c),                                 //
      ok_(false)                                 //
{
  uint8_t v[] = {pca9555::CONFIGURATION_0, 0, 0};
  ok_ = I2C::Result::OK == i2c_->write(pca9555::LEVEL_METER, v, sizeof(v));
}

satoh::LevelMeter::~LevelMeter() {}

bool satoh::LevelMeter::ok() const noexcept
{
  return ok_;
}

bool satoh::LevelMeter::set(uint8_t level, uint8_t n)
{
  if (7 < level || 1 < n)
  {
    return false;
  }
  constexpr uint8_t LEVEL[8] = {
      0b11111111, //
      0b11111110, //
      0b11111100, //
      0b11111000, //
      0b11110000, //
      0b11100000, //
      0b11000000, //
      0b10000000, //
  };
  constexpr uint8_t REG[2] = {
      pca9555::OUTPUT_0, //
      pca9555::OUTPUT_1, //
  };
  uint8_t v[] = {REG[n], LEVEL[level]};
  return I2C::Result::OK == i2c_->write(pca9555::LEVEL_METER, v, sizeof(v));
}
