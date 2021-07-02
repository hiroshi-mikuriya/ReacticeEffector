/// @file      device/ssd1306.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c.h"
#include <memory>

namespace satoh
{
class SSD1306;
}

class satoh::SSD1306
{
  /// I2C通信オブジェクト
  I2C *const i2c_;
  /// バッファ
  std::unique_ptr<uint8_t> buf_;
  /// 通信可否
  const bool ok_;
  /// @brief LCD初期化
  /// @retval true 書き込み成功
  /// @retval false 書き込み失敗
  bool init() const noexcept;
  /// @brief バッファをOLEDに書き込む
  void updateScreen();

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit SSD1306(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~SSD1306();
  /// @brief 通信可否
  /// @retval true 可
  /// @retval false 不可（デバイス繋がっていない等）
  bool ok() const noexcept;
};