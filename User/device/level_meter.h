/// @file      device/level_meter.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c.h"

namespace satoh
{
class LevelMeter;
}

/// @brief レベルメーター制御クラス
class satoh::LevelMeter
{
  I2C *const i2c_; ///< I2C通信オブジェクト
  bool ok_;        ///< デバイス状態

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit LevelMeter(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~LevelMeter();
  /// @brief デバイス状態取得
  /// @retval true 正常
  /// @retval false 異常
  bool ok() const noexcept;
  /// @brief レベルメーターを点灯させる
  /// @param[in] level レベル（0 - 7）
  /// @param[in] n
  ///   @arg 0 Left
  ///   @arg 1 Right
  /// @retval true 通信成功
  /// @retval false 通信失敗 or 引数エラー
  bool set(uint8_t level, uint8_t n);
};