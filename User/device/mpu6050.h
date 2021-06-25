/// @file      device/mpu6050.h
/// @author    Hiroshi Mikuriya
/// @copyright Copyright© 2021 Hiroshi Mikuriya
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include <cstdint>

namespace mpu6050
{
constexpr uint8_t SLAVE_ADDR = 0x68 << 1; ///< スレーブアドレス
constexpr uint8_t ACCEL_XOUT_H = 0x3B;    ///<
constexpr uint8_t ACCEL_XOUT_L = 0x3C;    ///<
constexpr uint8_t ACCEL_YOUT_H = 0x3D;    ///<
constexpr uint8_t ACCEL_YOUT_L = 0x3E;    ///<
constexpr uint8_t ACCEL_ZOUT_H = 0x3F;    ///<
constexpr uint8_t ACCEL_ZOUT_L = 0x40;    ///<
constexpr uint8_t TEMP_OUT_H = 0x41;      ///<
constexpr uint8_t TEMP_OUT_L = 0x42;      ///<
constexpr uint8_t GYRO_XOUT_H = 0x43;     ///<
constexpr uint8_t GYRO_XOUT_L = 0x44;     ///<
constexpr uint8_t GYRO_YOUT_H = 0x45;     ///<
constexpr uint8_t GYRO_YOUT_L = 0x46;     ///<
constexpr uint8_t GYRO_ZOUT_H = 0x47;     ///<
constexpr uint8_t GYRO_ZOUT_L = 0x48;     ///<
constexpr uint8_t PWR_MGMT_1 = 0x6B;      ///<
constexpr uint8_t PWR_MGMT_2 = 0x6C;      ///<
constexpr uint8_t WHOAMI = 0x75;          ///<
} // namespace mpu6050