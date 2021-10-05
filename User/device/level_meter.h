/// @file      device/level_meter.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c_device_base.hpp"

namespace satoh
{
class LevelMeter;
}

/// @brief レベルメーター制御クラス
class satoh::LevelMeter : public satoh::I2CDeviceBase
{
  /// デフォルトコンストラクタ削除
  LevelMeter() = delete;
  /// コピーコンストラクタ削除
  LevelMeter(LevelMeter const &) = delete;
  /// 代入演算子削除
  LevelMeter &operator=(LevelMeter const &) = delete;
  /// moveコンストラクタ削除
  LevelMeter(LevelMeter &&) = delete;
  /// move演算子削除
  LevelMeter &operator=(LevelMeter &&) = delete;

  const bool ok_; ///< デバイス状態
  uint8_t left_;  ///< レベルメーター（左）
  uint8_t right_; ///< レベルメーター（右）
  bool power_;    ///< Power LED
  bool tap_;      ///< Tap LED

  /// @brief デバイス初期化
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool init() const noexcept;

public:
  /// @brief コンストラクタ
  /// @param [in] i2c I2C通信オブジェクト
  explicit LevelMeter(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~LevelMeter() {}
  /// @brief デバイス状態を取得する @retval true 正常 @retval false 異常あり
  explicit operator bool() const noexcept override { return ok_; };
  /// @brief レベルメーター（左）のレベルを設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param [in] level レベル（0 - 7）
  void setLeft(uint8_t level) noexcept;
  /// @brief レベルメーター（右）のレベルを設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param [in] level レベル（0 - 7）
  void setRight(uint8_t level) noexcept;
  /// @brief POWER LEDの点灯・消灯を設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param [in] level
  ///    @arg true 点灯
  ///    @arg false 消灯
  void setPowerLed(bool level) noexcept;
  /// @brief TAP LEDの点灯・消灯を設定する
  /// @note この関数を呼んだだけでは点灯状態は変化しない
  /// @param [in] level
  ///    @arg true 点灯
  ///    @arg false 消灯
  void setTapLed(bool level) noexcept;
  /// @brief 設定をLEDに反映させる
  /// @retval true 通信成功
  /// @retval false 通信失敗 or 引数エラー
  bool show() const noexcept;
};