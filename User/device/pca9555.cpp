/// @file      device/pca9555.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "pca9555.h"

namespace
{
constexpr uint8_t INPUT_0 = 0x00;              ///<
constexpr uint8_t INPUT_1 = 0x01;              ///<
constexpr uint8_t OUTPUT_0 = 0x02;             ///<
constexpr uint8_t OUTPUT_1 = 0x03;             ///<
constexpr uint8_t POLARITY_INVERSION_0 = 0x04; ///<
constexpr uint8_t POLARITY_INVERSION_1 = 0x05; ///<
constexpr uint8_t CONFIGURATION_0 = 0x06;      ///<
constexpr uint8_t CONFIGURATION_1 = 0x07;      ///<
} // namespace

satoh::PCA9555::PCA9555(I2C *i2c, uint8_t slaveAddr) noexcept : i2c_(i2c), slaveAddr_(slaveAddr), ok_(false)
{
  switch (slaveAddr_)
  {
  case LEVEL_METER:
  {
    uint8_t v[] = {CONFIGURATION_0, 0, 0};
    ok_ = I2C::Result::OK == i2c_->write(slaveAddr_, v, sizeof(v));
    break;
  }
  case ROTARY_ENCODER:
  {
    uint8_t v[] = {CONFIGURATION_0, 0xFF, 0xFF};
    ok_ = I2C::Result::OK == i2c_->write(slaveAddr_, v, sizeof(v));
    break;
  }
  default:
    break;
  }
}

satoh::PCA9555::~PCA9555() {}

bool satoh::PCA9555::ok() const noexcept
{
  return ok_;
}

bool satoh::PCA9555::write(uint8_t const (&d)[2]) const noexcept
{
  uint8_t v[] = {OUTPUT_0, d[0], d[1]};
  return I2C::Result::OK == i2c_->write(slaveAddr_, v, sizeof(v));
}

bool satoh::PCA9555::read(uint8_t (&d)[2]) const noexcept
{
  uint8_t reg = INPUT_0;
  return I2C::Result::OK == i2c_->write(slaveAddr_, &reg, sizeof(reg)) && //
         I2C::Result::OK == i2c_->read(slaveAddr_, d, sizeof(d))          //
      ;
}
