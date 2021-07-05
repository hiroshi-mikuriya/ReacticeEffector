/// @file      device/pca9635.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include "i2c_device_base.hpp"

namespace satoh
{
class PCA9635;
}

/// @brief PCA9635制御クラス（エフェクトLED）
class satoh::PCA9635 : public satoh::I2CDeviceBase
{
  /// 通信可否
  const bool ok_;
  /// @brief I2C書き込み
  /// @param[in] reg 書き込み先のレジスタ
  /// @param[in] bytes 書き込みデータ
  /// @param[in] size 書き込みサイズ
  bool write(uint8_t reg, uint8_t const *bytes, uint32_t size) const noexcept;
  /// @brief デバイス初期化
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool init() const noexcept;

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit PCA9635(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~PCA9635();
  /// @brief 通信可否
  /// @retval true 可
  /// @retval false 不可（デバイス繋がっていない等）
  bool ok() const noexcept override;
  /// @brief LEDの色値を設定
  /// @param[in] rgb 色値
  /// @param[in] n LED番号（0 - 3）
  /// @retval true 設定成功
  /// @retval false I2C通信失敗、LED番号不正等
  bool set(RGB const &rgb, uint8_t n) noexcept;
};