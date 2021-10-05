/// @file      device/mpu6050.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c_device_base.hpp"

namespace satoh
{
class Gyro;

constexpr uint8_t MPU6050 = 0x68;
constexpr uint8_t ICM20602 = 0x69;
} // namespace satoh

/// @brief ジャイロセンサ（MPU6050 / ICM20602）と通信するクラス
class satoh::Gyro : public satoh::I2CDeviceBase
{
  /// デフォルトコンストラクタ削除
  Gyro() = delete;
  /// コピーコンストラクタ削除
  Gyro(Gyro const &) = delete;
  /// 代入演算子削除
  Gyro &operator=(Gyro const &) = delete;
  /// moveコンストラクタ削除
  Gyro(Gyro &&) = delete;
  /// move演算子削除
  Gyro &operator=(Gyro &&) = delete;
  /// 通信可否
  const bool ok_;
  /// @brief センサ初期化
  /// @retval true 書き込み成功
  /// @retval false 書き込み失敗
  bool init() const noexcept;

public:
  /// @brief コンストラクタ
  /// @param [in] i2c I2C通信オブジェクト
  /// @param [in] slaveAddr ジャイロセンサのスレーブアドレス
  explicit Gyro(I2C *i2c, uint8_t slaveAddr) noexcept;
  /// @brief デストラクタ
  virtual ~Gyro() {}
  /// @brief デバイス状態を取得する @retval true 正常 @retval false 異常あり
  explicit operator bool() const noexcept override { return ok_; };
  /// @brief 加速度値取得
  /// @param [out] acc 加速度値格納先
  /// @retval true 取得成功
  /// @retval false 取得失敗
  bool getAccel(int16_t (&acc)[3]) const noexcept;
  /// @brief ジャイロ値取得
  /// @param [out] gyro ジャイロ値格納先
  /// @retval true 取得成功
  /// @retval false 取得失敗
  bool isGyroEnabled(int16_t (&gyro)[3]) const noexcept;
  /// @brief 加速度値とジャイロ値取得
  /// @param [out] acc 加速度値格納先
  /// @param [out] gyro ジャイロ値格納先
  /// @retval true 取得成功
  /// @retval false 取得失敗
  bool getAccelGyro(int16_t (&acc)[3], int16_t (&gyro)[3]) const noexcept;
};