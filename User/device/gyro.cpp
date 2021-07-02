/// @file      device/gyro.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "gyro.h"
#include "common/big_endian.hpp"

namespace
{
constexpr uint8_t SMPLRT_DIV = 0x19;
constexpr uint8_t CONFIG = 0x1A;
constexpr uint8_t GYRO_CONFIG = 0x1B;
constexpr uint8_t ACCEL_CONFIG = 0x1C;
constexpr uint8_t FIFO_EN = 0x23;
constexpr uint8_t INT_PIN_CFG = 0x37;
constexpr uint8_t INT_ENABLE = 0x38;
constexpr uint8_t INT_STATUS = 0x3A;
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
constexpr uint8_t SIGNAL_PATH_RESET = 0x68;
constexpr uint8_t USER_CTRL = 0x6A;
constexpr uint8_t PWR_MGMT_1 = 0x6B;
constexpr uint8_t PWR_MGMT_2 = 0x6C;
constexpr uint8_t WHOAMI = 0x75;
} // namespace

bool satoh::Gyro::init() const noexcept
{
  return true                        //
         && write(SMPLRT_DIV, 9)     // ローパスフィルタ―を適用して10msでサンプリングすることを想定
         && write(CONFIG, 2)         // 100Hzぐらいのフィルタ―にすればいいかな。
         && write(GYRO_CONFIG, 0)    // 回転させないから角度は最小で± 250 °/ s
         && write(ACCEL_CONFIG, 8)   // 加速度レンジは±4gぐらい？
         && write(FIFO_EN, 0)        // FIFOは使わず、センサー値は各センサーのレジスタを読み出す
         && write(INT_PIN_CFG, 0xD0) // 割り込みはオープンドレインの負論理、リードクリア
         && write(INT_ENABLE, 1)     // データの準備ができたら割り込みを発生させる
         && write(USER_CTRL, 0)      // FIFOは使わない
         && write(PWR_MGMT_1, 0)     //
      ;
}

bool satoh::Gyro::write(uint8_t reg, uint8_t v) const noexcept
{
  uint8_t a[2] = {reg, v};
  return I2C::Result::OK == i2c_->write(slaveAddr_, a, sizeof(a));
}
bool satoh::Gyro::read(uint8_t reg, uint8_t *buffer, uint32_t size) const noexcept
{
  return I2C::Result::OK == i2c_->write(slaveAddr_, &reg, sizeof(reg)) && //
         I2C::Result::OK == i2c_->read(slaveAddr_, buffer, size)          //
      ;
}

satoh::Gyro::Gyro(I2C *i2c, uint8_t slaveAddr) noexcept //
    : i2c_(i2c),                                        //
      slaveAddr_(slaveAddr),                            //
      ok_(init())                                       //
{
}
satoh::Gyro::~Gyro() {}
bool satoh::Gyro::ok() const noexcept
{
  return ok_;
}
bool satoh::Gyro::getAccel(int16_t (&acc)[3]) const noexcept
{
  uint8_t buf[6] = {0};
  bool res = read(ACCEL_XOUT_H, buf, sizeof(buf));
  if (res)
  {
    acc[0] = BE<int16_t>::get(buf + 0);
    acc[1] = BE<int16_t>::get(buf + 2);
    acc[2] = BE<int16_t>::get(buf + 4);
  }
  return res;
}
bool satoh::Gyro::getGyro(int16_t (&gyro)[3]) const noexcept
{
  uint8_t buf[6] = {0};
  bool res = read(GYRO_XOUT_H, buf, sizeof(buf));
  if (res)
  {
    gyro[0] = BE<int16_t>::get(buf + 0);
    gyro[1] = BE<int16_t>::get(buf + 2);
    gyro[2] = BE<int16_t>::get(buf + 4);
  }
  return res;
}
bool satoh::Gyro::getAccelGyro(int16_t (&acc)[3], int16_t (&gyro)[3]) const noexcept
{
  uint8_t buf[6 + 2 + 6] = {0};
  bool res = read(ACCEL_XOUT_H, buf, sizeof(buf));
  if (res)
  {
    acc[0] = BE<int16_t>::get(buf + 0);
    acc[1] = BE<int16_t>::get(buf + 2);
    acc[2] = BE<int16_t>::get(buf + 4);
    gyro[0] = BE<int16_t>::get(buf + 8);
    gyro[1] = BE<int16_t>::get(buf + 10);
    gyro[2] = BE<int16_t>::get(buf + 12);
  }
  return res;
}