/// @file      device/pca9555.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "level_meter.h"
#include "pca9555.h"

namespace
{
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
}

bool satoh::LevelMeter::init() const noexcept
{
  uint8_t v[] = {pca9555::CONFIGURATION_0, 0, 0};
  return write(v, sizeof(v));
}

satoh::LevelMeter::LevelMeter(I2C *i2c) noexcept //
    : I2CDeviceBase(i2c, pca9555::LEVEL_METER),  //
      ok_(init()),                               //
      left_(LEVEL[0]),                           //
      right_(LEVEL[0]),                          //
      power_(false),                             //
      modulation_(false)                         //
{
}

satoh::LevelMeter::~LevelMeter() {}

bool satoh::LevelMeter::ok() const noexcept
{
  return ok_;
}

void satoh::LevelMeter::setLeft(uint8_t level) noexcept
{
  if (level < sizeof(LEVEL))
  {
    left_ = LEVEL[level];
  }
}
void satoh::LevelMeter::setRight(uint8_t level) noexcept
{
  if (level < sizeof(LEVEL))
  {
    right_ = LEVEL[level];
  }
}
void satoh::LevelMeter::setPower(bool level) noexcept
{
  power_ = level;
}
void satoh::LevelMeter::setModulation(bool level) noexcept
{
  modulation_ = level;
}
bool satoh::LevelMeter::show() const noexcept
{
  uint8_t v[] = {pca9555::OUTPUT_0, left_, right_};
  constexpr uint8_t MASK = 0b01111111;
  if (power_)
  {
    v[2] &= MASK;
  }
  if (modulation_)
  {
    v[1] &= MASK;
  }
  return write(v, sizeof(v));
}
