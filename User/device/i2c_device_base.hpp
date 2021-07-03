/// @file      device/i2c_device_base.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "peripheral/i2c.h"

namespace satoh
{
class I2CDeviceBase;
}

/// @brief I2Cデバイス基底クラス
class satoh::I2CDeviceBase
{
  I2C *const i2c_;          ///< I2C通信オブジェクト
  const uint8_t slaveAddr_; ///< スレーブアドレス

protected:
  /// @brief デバイスに書き込む
  /// @param[in] bytes 書き込みデータの先頭ポインタ
  /// @param[in] size 書き込むバイト数
  /// @retval true 成功
  /// @retval false 失敗
  bool write(uint8_t const *bytes, uint32_t size) const noexcept { return I2C::OK == i2c_->write(slaveAddr_, bytes, size); }
  /// @brief デバイスから読み込む
  /// @param[in] reg 読み込み先のレジスタ番号
  /// @param[in] buf 読み込んだデータを格納するバッファ
  /// @param[in] size 読み込むバイト数
  /// @retval true 成功
  /// @retval false 失敗
  bool read(uint8_t reg, uint8_t *buffer, uint32_t size) const noexcept
  {
    return I2C::OK == i2c_->write(slaveAddr_, &reg, sizeof(reg), false) && I2C::OK == i2c_->read(slaveAddr_, buffer, size);
  }

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  /// @param[in] slaveAddr スレーブアドレス
  explicit I2CDeviceBase(I2C *i2c, uint8_t slaveAddr) noexcept : i2c_(i2c), slaveAddr_(slaveAddr << 1) {}
  /// @brief デストラクタ
  virtual ~I2CDeviceBase() {}
  /// @brief デバイス状態取得
  /// @retval true 正常
  /// @retval false 異常
  virtual bool ok() const noexcept = 0;
};