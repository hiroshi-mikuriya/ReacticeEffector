/// @file      device/ssd1306.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/alloc.hpp"
#include "i2c_device_base.hpp"
#include "message/type.h"

namespace satoh
{
class SSD1306;
}

/// @brief SSD1306制御クラス（OLED）
class satoh::SSD1306 : public satoh::I2CDeviceBase
{
  /// デフォルトコンストラクタ削除
  SSD1306() = delete;
  /// コピーコンストラクタ削除
  SSD1306(SSD1306 const &) = delete;
  /// 代入演算子削除
  SSD1306 &operator=(SSD1306 const &) = delete;
  /// moveコンストラクタ削除
  SSD1306(SSD1306 &&) = delete;
  /// move演算子削除
  SSD1306 &operator=(SSD1306 &&) = delete;
  /// 表示用バッファ
  UniquePtr<uint8_t> dispbuf_;
  /// 通信用バッファ
  UniquePtr<uint8_t> txbuf_;
  /// 通信可否
  const bool ok_;
  /// 選択中のパラメータ番号
  uint8_t selectedParam_;
  /// @brief LCD初期化 @retval true 通信成功 @retval false 通信失敗
  bool init() const noexcept;
  /// @brief リセット処理 @retval true 通信成功 @retval false 通信失敗
  bool reset() const noexcept;
  /// @brief ページ分のバッファをOLEDに書き込む
  /// @param[in] page ページ番号（0〜7）
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool sendBufferToDevice(uint8_t page) noexcept;
  /// @brief 全ページ分のバッファをOLEDに書き込む @retval true 通信成功 @retval false 通信失敗
  bool sendBufferToDevice() noexcept;

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit SSD1306(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~SSD1306();
  /// @brief 通信可否 @retval true 可 @retval false 不可（デバイス繋がっていない等）
  bool ok() const noexcept override;
  /// @brief パラメータ編集画面を表示する @param[in] src 画面設定 @retval true 通信成功 @retval false 通信失敗
  bool update(msg::OLED_DISP_EFFECTOR const &src) noexcept;
  /// @brief 演奏モード画面を表示する @param[in] src 画面設定 @retval true 通信成功 @retval false 通信失敗
  bool update(msg::OLED_DISP_BANK const &src) noexcept;
  /// @brief コンファーム画面を表示する @param[in] src 画面設定 @retval true 通信成功 @retval false 通信失敗
  bool update(msg::OLED_DISP_CONFIRM const &src) noexcept;
  /// @brief テキスト画面を表示する @param[in] src 画面設定 @retval true 通信成功 @retval false 通信失敗
  bool update(msg::OLED_DISP_TEXT const &src) noexcept;
};
