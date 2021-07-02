/// @file      message/type.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include <cstdint>

namespace satoh
{
namespace msg
{
/// @brief メッセージカテゴリ定義
namespace cat
{
constexpr uint8_t GYRO = 1 << 4;
constexpr uint8_t LED = 2 << 4;
constexpr uint8_t ENCODER = 3 << 4;
constexpr uint8_t KEY = 4 << 4;
constexpr uint8_t OLED = 5 << 4;
constexpr uint8_t USB = 6 << 4;
} // namespace cat

/// ジャイロ - 取得依頼
constexpr uint8_t GYRO_GET_REQ = 1 | cat::GYRO;
/// ジャイロ - 加速度・ジャイロ値通知
constexpr uint8_t GYRO_NOTIFY = 2 | cat::GYRO;
/// LED - レベルメーター（左右）更新依頼
constexpr uint8_t LED_LEVEL_UPDATE_REQ = 1 | cat::LED;
/// LED - Power/Modulation LED点灯状態変更依頼
constexpr uint8_t LED_SIMPLE_REQ = 2 | cat::LED;
/// LED - EFFECT LED点灯状態変更依頼
constexpr uint8_t LED_EFFECT_REQ = 3 | cat::LED;
/// ENCODER - エンコーダ値取得依頼
constexpr uint8_t ENCODER_GET_REQ = 1 | cat::ENCODER;
/// ENCODER - エンコーダ値通知
constexpr uint8_t ENCODER_NOTIFY = 2 | cat::ENCODER;
/// キー入力 - 取得依頼
constexpr uint8_t KEY_GET_REQ = 1 | cat::KEY;
/// キー入力 - キー取得値通知
constexpr uint8_t KEY_NOTIFY = 2 | cat::KEY;
/// OLED - 画面更新依頼
constexpr uint8_t OLED_UPDATE_REQ = 1 | cat::OLED;
/// USB - 送信依頼
constexpr uint8_t USB_TX_REQ = 1 | cat::USB;
/// USB - 受信通知
constexpr uint8_t USB_RX_NOTIFY = 2 | cat::USB;

struct ACC_GYRO;
struct LED_LEVEL;
struct LED_SIMPLE;
struct LED_EFFECT;
} // namespace msg
} // namespace satoh

/// @brief ジャイロ - 加速度・ジャイロ値通知型
struct satoh::msg::ACC_GYRO
{
  int16_t acc[3];
  int16_t gyro[3];
};
/// @brief LED - レベルメーター（左右）更新依頼型
struct satoh::msg::LED_LEVEL
{
  uint8_t left;  ///< レベルメーター左（0 - 7）
  uint8_t right; ///< レベルメーター右（0 - 7）
};
/// @brief Power/Modulation LED点灯状態変更依頼型
struct satoh::msg::LED_SIMPLE
{
  uint8_t led; ///< @arg 0 POWER @arg 1 MODULATION
  bool level;  ///< @arg true 点灯 @arg false 消灯
};
/// @brief LED - EFFECT LED点灯状態変更依頼型
struct satoh::msg::LED_EFFECT
{
  uint8_t led; ///< LED番号（0 - 3）
  RGB rgb;     ///< 色
};