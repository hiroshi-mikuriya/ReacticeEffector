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
constexpr uint8_t SHIFT = 4;
constexpr uint8_t GYRO = 1 << SHIFT;
constexpr uint8_t LED = 2 << SHIFT;
constexpr uint8_t ENCODER = 3 << SHIFT;
constexpr uint8_t KEY = 4 << SHIFT;
constexpr uint8_t OLED = 5 << SHIFT;
constexpr uint8_t USB = 6 << SHIFT;
constexpr uint8_t NEOPIXEL = 7 << SHIFT;
} // namespace cat

/// ジャイロ - 取得依頼
constexpr uint8_t GYRO_GET_REQ = 1 | cat::GYRO;
/// ジャイロ - 加速度・ジャイロ値通知
constexpr uint8_t GYRO_NOTIFY = 2 | cat::GYRO;
/// LED - レベルメーター（左右）更新依頼
constexpr uint8_t LED_LEVEL_UPDATE_REQ = 1 | cat::LED;
/// LED - Power/Modulation LED点灯状態変更依頼
constexpr uint8_t LED_SIMPLE_REQ = 2 | cat::LED;
/// LED - EFFECT LED点灯状態変更依頼（LED指定）
constexpr uint8_t LED_EFFECT_REQ = 3 | cat::LED;
/// LED - EFFECT LED点灯状態変更依頼（全LED）
constexpr uint8_t LED_ALL_EFFECT_REQ = 4 | cat::LED;
/// ENCODER - エンコーダ値取得依頼
constexpr uint8_t ENCODER_GET_REQ = 1 | cat::ENCODER;
/// ENCODER - ロータリーエンコーダ値通知
constexpr uint8_t ROTARY_ENCODER_NOTIFY = 2 | cat::ENCODER;
/// ENCODER - エフェクトキー変化通知
constexpr uint8_t EFFECT_KEY_CHANGED_NOTIFY = 3 | cat::ENCODER;
/// キー入力 - 取得依頼
constexpr uint8_t MODE_KEY_GET_REQ = 1 | cat::KEY;
/// キー入力 - キー取得値通知
constexpr uint8_t MODE_KEY_NOTIFY = 2 | cat::KEY;
/// OLED - 画面更新依頼
constexpr uint8_t OLED_UPDATE_REQ = 1 | cat::OLED;
/// USB - 送信依頼
constexpr uint8_t USB_TX_REQ = 1 | cat::USB;
/// USB - 受信通知
constexpr uint8_t USB_RX_NOTIFY = 2 | cat::USB;
/// NeoPixel - 点灯パターン指定
constexpr uint8_t NEO_PIXEL_SET_PATTERN = 1 | cat::NEOPIXEL;
/// NeoPixel - 点灯スピード指定
constexpr uint8_t NEO_PIXEL_SET_SPEED = 2 | cat::NEOPIXEL;

struct ACC_GYRO;
struct LED_LEVEL;
struct LED_SIMPLE;
struct LED_EFFECT;
struct LED_ALL_EFFECT;
struct ROTARY_ENCODER;
struct EFFECT_KEY;
struct MODE_KEY;
struct NEO_PIXEL_PATTERN;
struct NEO_PIXEL_SPEED;

constexpr uint8_t BUTTON_UP = 0;   ///< ボタン離し中
constexpr uint8_t BUTTON_DOWN = 1; ///< ボタン押下中
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
/// @brief LED - EFFECT LED点灯状態変更依頼型（LED指定）
struct satoh::msg::LED_EFFECT
{
  uint8_t led; ///< LED番号（0 - 3）
  RGB rgb;     ///< 色
};
/// @brief LED - EFFECT LED点灯状態変更依頼型（全LED）
struct satoh::msg::LED_ALL_EFFECT
{
  RGB rgb[4]; ///< 色
};
/// @brief ロータリーエンコーダ値通知
struct satoh::msg::ROTARY_ENCODER
{
  /// 回転量の変化
  /// @arg -1 左方向回転
  /// @arg 0 変化なし
  /// @arg 1 右方向回転
  int8_t angleDiff[4];
};
/// @brief エフェクトキー変化通知
struct satoh::msg::EFFECT_KEY
{
  /// キー状態
  /// @arg BUTTON_UP ボタン離し中
  /// @arg BUTTON_DOWN ボタン押下中
  uint8_t button[4];
};
/// @brief モードキー変化通知
struct satoh::msg::MODE_KEY
{
  /// TAPキー状態
  /// @arg BUTTON_UP ボタン離し中
  /// @arg BUTTON_DOWN ボタン押下中
  uint8_t tap;
  /// UPキー状態
  /// @arg BUTTON_UP ボタン離し中
  /// @arg BUTTON_DOWN ボタン押下中
  uint8_t up;
  /// DOWNキー状態
  /// @arg BUTTON_UP ボタン離し中
  /// @arg BUTTON_DOWN ボタン押下中
  uint8_t down;
  /// OKキー状態
  /// @arg BUTTON_UP ボタン離し中
  /// @arg BUTTON_DOWN ボタン押下中
  uint8_t ok;
  /// リターンキー状態
  /// @arg BUTTON_UP ボタン離し中
  /// @arg BUTTON_DOWN ボタン押下中
  uint8_t rtn;
  /// RE1キー状態
  /// @arg BUTTON_UP ボタン離し中
  /// @arg BUTTON_DOWN ボタン押下中
  uint8_t re1;
};
/// NeoPixel - 点灯パターン指定型
struct satoh::msg::NEO_PIXEL_PATTERN
{
  /// 点灯パターン
  RGB rgb[6];
};
/// NeoPixel - 点灯スピード指定型
struct satoh::msg::NEO_PIXEL_SPEED
{
  /// インターバル（ミリ秒）
  uint32_t interval;
};
