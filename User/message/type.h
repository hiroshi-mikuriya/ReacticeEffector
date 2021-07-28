/// @file      message/type.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include "constant.h"
#include "effector/effector_base.h"

namespace satoh
{
namespace msg
{
/// @brief メッセージID型
typedef uint16_t ID;
/// @brief メッセージカテゴリ定義
namespace cat
{
constexpr ID SHIFT = 12;
constexpr ID GYRO = 1 << SHIFT;
constexpr ID LED = 2 << SHIFT;
constexpr ID ENCODER = 3 << SHIFT;
constexpr ID KEY = 4 << SHIFT;
constexpr ID OLED = 5 << SHIFT;
constexpr ID USB = 6 << SHIFT;
constexpr ID NEOPIXEL = 7 << SHIFT;
constexpr ID SOUND = 8 << SHIFT;
} // namespace cat

/// ジャイロ - 取得依頼
constexpr ID GYRO_GET_REQ = 1 | cat::GYRO;
/// ジャイロ - 加速度・ジャイロ値通知
constexpr ID GYRO_NOTIFY = 2 | cat::GYRO;
/// LED - レベルメーター（左右）更新依頼
constexpr ID LED_LEVEL_UPDATE_REQ = 1 | cat::LED;
/// LED - Power/Modulation LED点灯状態変更依頼
constexpr ID LED_SIMPLE_REQ = 2 | cat::LED;
/// LED - EFFECT LED点灯状態変更依頼（LED指定）
constexpr ID LED_EFFECT_REQ = 3 | cat::LED;
/// LED - EFFECT LED点灯状態変更依頼（全LED）
constexpr ID LED_ALL_EFFECT_REQ = 4 | cat::LED;
/// ENCODER - エンコーダ値取得依頼
constexpr ID ENCODER_GET_REQ = 1 | cat::ENCODER;
/// ENCODER - ロータリーエンコーダ値通知
constexpr ID ROTARY_ENCODER_NOTIFY = 2 | cat::ENCODER;
/// ENCODER - エフェクトキー変化通知
constexpr ID EFFECT_KEY_CHANGED_NOTIFY = 3 | cat::ENCODER;
/// キー入力 - 取得依頼
constexpr ID MODE_KEY_GET_REQ = 1 | cat::KEY;
/// キー入力 - キー取得値通知
constexpr ID MODE_KEY_NOTIFY = 2 | cat::KEY;
/// OLED - エフェクターパラメータ一一覧表示依頼
constexpr ID OLED_DISP_EFFECTOR_REQ = 1 | cat::OLED;
/// OLED - バンク画面表示依頼
constexpr ID OLED_DISP_BANK_REQ = 2 | cat::OLED;
/// OLED - コンファーム画面表示依頼
constexpr ID OLED_DISP_CONFIRM_REQ = 3 | cat::OLED;
/// OLED - テキスト表示依頼
constexpr ID OLED_DISP_TEXT_REQ = 4 | cat::OLED;
/// USB - 送信依頼
constexpr ID USB_TX_REQ = 1 | cat::USB;
/// USB - 受信通知
constexpr ID USB_RX_NOTIFY = 2 | cat::USB;
/// NeoPixel - 点灯パターン指定
constexpr ID NEO_PIXEL_SET_PATTERN = 1 | cat::NEOPIXEL;
/// NeoPixel - 点灯スピード指定
constexpr ID NEO_PIXEL_SET_SPEED = 2 | cat::NEOPIXEL;
/// Sound - DMA半分受信完了通知
constexpr ID SOUND_DMA_HALF_NOTIFY = 1 | cat::SOUND;
/// Sound - DMA全部受信完了通知
constexpr ID SOUND_DMA_CPLT_NOTIFY = 2 | cat::SOUND;
/// Sound - エフェクター変更要求
constexpr ID SOUND_CHANGE_EFFECTOR_REQ = 3 | cat::SOUND;

struct ACC_GYRO;
struct LED_LEVEL;
struct LED_SIMPLE;
struct LED_EFFECT;
struct LED_ALL_EFFECT;
struct ROTARY_ENCODER;
struct EFFECT_KEY;
struct MODE_KEY;
struct OLED_DISP_EFFECTOR;
struct OLED_DISP_BANK;
struct OLED_DISP_CONFIRM;
struct OLED_DISP_TEXT;
struct NEO_PIXEL_PATTERN;
struct NEO_PIXEL_SPEED;
struct SOUND_EFFECTOR;

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
  RGB rgb[EFFECT_LED_COUNT]; ///< 色
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
  uint8_t button[EFFECT_BUTTON_COUNT];
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
/// OLED - エフェクターパラメータ一一覧表示依頼型
struct satoh::msg::OLED_DISP_EFFECTOR
{
  /// 表示するエフェクター
  fx::EffectorBase *fx;
  /// パッチ番号
  uint8_t patch;
  /// 選択中のパラメータ番号
  uint8_t selectedParam;
};
/// OLED - バンク画面表示依頼型
struct satoh::msg::OLED_DISP_BANK
{
  /// バンク番号
  uint8_t bank;
  /// パッチ番号
  uint8_t patch;
  /// 表示するエフェクター
  fx::EffectorBase *fx[MAX_EFFECTOR_COUNT];
  /// エディットモードフラグ
  bool editMode;
  /// 選択中のエフェクト番号（editMode = true のときのみ有効）
  uint8_t selectedFx;
};
/// OLED - コンファーム画面表示依頼型
struct satoh::msg::OLED_DISP_CONFIRM
{
  char msg1[19]; ///< テキスト１行目（末尾の0含む）
  char msg2[19]; ///< テキスト２行目（末尾の0含む）
  bool yes;      ///< @arg YES選択 @arg NO選択
};
/// OLED - テキスト表示依頼型
struct satoh::msg::OLED_DISP_TEXT
{
  char msg1[19]; ///< テキスト１行目（末尾の0含む）
  char msg2[19]; ///< テキスト２行目（末尾の0含む）
  char msg3[19]; ///< テキスト３行目（末尾の0含む）
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
/// Sound - エフェクター変更要求型
struct satoh::msg::SOUND_EFFECTOR
{
  /// エフェクタークラスのポインタ
  fx::EffectorBase *fx[MAX_EFFECTOR_COUNT];
};
