/// @file      device/gyro.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "gyro.h"

namespace
{
constexpr uint8_t ACCEL_XOUT_H = 0x3B;
constexpr uint8_t ACCEL_XOUT_L = 0x3C;
constexpr uint8_t ACCEL_YOUT_H = 0x3D;
constexpr uint8_t ACCEL_YOUT_L = 0x3E;
constexpr uint8_t ACCEL_ZOUT_H = 0x3F;
constexpr uint8_t ACCEL_ZOUT_L = 0x40;
constexpr uint8_t TEMP_OUT_H = 0x41;
constexpr uint8_t TEMP_OUT_L = 0x42;
constexpr uint8_t GYRO_XOUT_H = 0x43;
constexpr uint8_t GYRO_XOUT_L = 0x44;
constexpr uint8_t GYRO_YOUT_H = 0x45;
constexpr uint8_t GYRO_YOUT_L = 0x46;
constexpr uint8_t GYRO_ZOUT_H = 0x47;
constexpr uint8_t GYRO_ZOUT_L = 0x48;
constexpr uint8_t PWR_MGMT_1 = 0x6B;
constexpr uint8_t PWR_MGMT_2 = 0x6C;
constexpr uint8_t WHOAMI = 0x75;

/// @brief ビッグエンディアン形式で16ビット整数へ変換する
/// @param[in] u データ列
/// @return 16ビット整数
inline int16_t to16be(uint8_t const *u)
{
  return static_cast<int16_t>((u[0] << 8) + u[1]);
}
} // namespace

bool satoh::Gyro::write(uint8_t reg, uint8_t v)
{
  uint8_t a[2] = {reg, v};
  return I2C::Result::OK == i2c_->write(slaveAddr_, a, sizeof(a));
}
bool satoh::Gyro::read(uint8_t reg, uint8_t *buffer, uint32_t size)
{
  return I2C::Result::OK == i2c_->write(slaveAddr_, &reg, sizeof(reg)) && //
         I2C::Result::OK == i2c_->read(slaveAddr_, buffer, size)          //
      ;
}

satoh::Gyro::Gyro(I2C *i2c, uint8_t slaveAddr) noexcept //
    : i2c_(i2c),                                        //
      slaveAddr_(slaveAddr),                            //
      ok_(write(PWR_MGMT_1, 0))                         //
{
}
satoh::Gyro::~Gyro() {}
bool satoh::Gyro::ok() const noexcept
{
  return ok_;
}
bool satoh::Gyro::getAccell(int16_t (&acc)[3]) noexcept
{
  uint8_t buf[6] = {0};
  bool res = read(ACCEL_XOUT_H, buf, sizeof(buf));
  if (res)
  {
    acc[0] = to16be(buf + 0);
    acc[1] = to16be(buf + 2);
    acc[2] = to16be(buf + 4);
  }
  return res;
}
bool satoh::Gyro::getGyro(int16_t (&gyro)[3]) noexcept
{
  uint8_t buf[6] = {0};
  bool res = read(GYRO_XOUT_H, buf, sizeof(buf));
  if (res)
  {
    gyro[0] = to16be(buf + 0);
    gyro[1] = to16be(buf + 2);
    gyro[2] = to16be(buf + 4);
  }
  return res;
}
bool satoh::Gyro::getAccelGyro(int16_t (&acc)[3], int16_t (&gyro)[3]) noexcept
{
  uint8_t buf[6 + 2 + 6] = {0};
  bool res = read(ACCEL_XOUT_H, buf, sizeof(buf));
  if (res)
  {
    acc[0] = to16be(buf + 0);
    acc[1] = to16be(buf + 2);
    acc[2] = to16be(buf + 4);
    gyro[0] = to16be(buf + 8);
    gyro[1] = to16be(buf + 10);
    gyro[2] = to16be(buf + 12);
  }
  return res;
}