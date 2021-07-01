/// @file      message/type.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

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

/// ジャイロ - 更新依頼
constexpr uint8_t GYRO_UPDATE_REQ = 1 | cat::GYRO;
/// ジャイロ - 加速度・ジャイロ値通知
constexpr uint8_t GYRO_NOTIFY = 2 | cat::GYRO;
/// LED - レベルメーター（左右）更新依頼
constexpr uint8_t LED_LEVEL_UPDATE_REQ = 1 | cat::LED;
/// LED - POWER LED点灯状態変更依頼
constexpr uint8_t LED_POWER_REQ = 2 | cat::LED;
/// LED - MODULATION LED点灯状態変更依頼
constexpr uint8_t LED_MODULATION_REQ = 3 | cat::LED;
/// LED - EFFECT LED点灯状態変更依頼
constexpr uint8_t LED_EFFECT_REQ = 4 | cat::LED;
/// ENCODER - エンコーダ値更新依頼
constexpr uint8_t ENCODER_UPDATE_REQ = 1 | cat::ENCODER;
/// ENCODER - エンコーダ値通知
constexpr uint8_t ENCODER_NOTIFY = 2 | cat::ENCODER;
/// キー入力 - 更新依頼
constexpr uint8_t KEY_UPDATE_REQ = 1 | cat::KEY;
/// キー入力 - キー取得値通知
constexpr uint8_t KEY_NOTIFY = 2 | cat::KEY;
/// OLED - 画面更新依頼
constexpr uint8_t OLED_UPDATE_REQ = 1 | cat::OLED;
/// USB - 送信依頼
constexpr uint8_t USB_TX_REQ = 1 | cat::USB;
/// USB - 受信通知
constexpr uint8_t USB_RX_NOTIFY = 2 | cat::USB;
} // namespace msg
} // namespace satoh