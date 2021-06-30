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
  I2C *const i2c_;  ///< I2C通信オブジェクト
  bool ok_;         ///< デバイス状態
  uint8_t left_;    ///< レベルメーター（左）
  uint8_t right_;   ///< レベルメーター（右）
  bool power_;      ///< Power LED
  bool modulation_; ///< Modulation LED

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
  /// @brief レベルメーター（左）のレベルを設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param[in] level レベル（0 - 7）
  void setLeft(uint8_t level) noexcept;
  /// @brief レベルメーター（右）のレベルを設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param[in] level レベル（0 - 7）
  void setRight(uint8_t level) noexcept;
  /// @brief Power LEDの点灯・消灯を設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param[in] level
  ///    @arg true 点灯
  ///    @arg false 消灯
  void setPower(bool level) noexcept;
  /// @brief Modulation LEDの点灯・消灯を設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param[in] level
  ///    @arg true 点灯
  ///    @arg false 消灯
  void setModulation(bool level) noexcept;
  /// @brief 設定をLEDに反映させる
  /// @retval true 通信成功
  /// @retval false 通信失敗 or 引数エラー
  bool show() const noexcept;
};