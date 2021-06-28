/// @file      device/pca9555.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c.h"

namespace satoh
{
class PCA9555;

constexpr uint8_t LEVEL_METER = 0x20 << 1;
constexpr uint8_t ROTARY_ENCODER = 0x21 << 1;
} // namespace satoh

class satoh::PCA9555
{
  /// I2C通信オブジェクト
  I2C *const i2c_;
  /// スレーブアドレス
  const uint8_t slaveAddr_;
  /// デバイス状態
  bool ok_;

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit PCA9555(I2C *i2c, uint8_t slaveAddr) noexcept;
  /// @brief デストラクタ
  virtual ~PCA9555();
  /// @brief デバイス状態取得
  /// @retval true 正常
  /// @retval false 異常
  bool ok() const noexcept;
  /// @brief 出力ポートに書き込む
  /// @param[in] d 書き込む値
  /// @retval true 成功
  /// @retval false 失敗
  bool write(uint8_t const (&d)[2]) const noexcept;
  /// @brief 入力ポートを読み込む
  /// @param[out] d 読み込んだ値の格納先
  /// @retval true 成功
  /// @retval false 失敗
  bool read(uint8_t (&d)[2]) const noexcept;
};