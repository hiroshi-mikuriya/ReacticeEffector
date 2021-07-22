/// @file      device/ssd1306.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c_device_base.hpp"
#include "message/type.h"
#include <memory>

namespace satoh
{
class SSD1306;
}

/// @brief SSD1306制御クラス（OLED）
class satoh::SSD1306 : public satoh::I2CDeviceBase
{
  /// 表示用バッファ
  std::unique_ptr<uint8_t> dispbuf_;
  /// 通信用バッファ
  std::unique_ptr<uint8_t> txbuf_;
  /// 通信可否
  const bool ok_;
  /// 表示対象エフェクター
  fx::EffectorBase const *effector_;
  /// パッチ番号
  uint8_t patch_;
  /// 選択中のパラメータ番号
  uint8_t selectedParam_;
  /// @brief LCD初期化
  /// @retval true 書き込み成功
  /// @retval false 書き込み失敗
  bool init() const noexcept;
  /// @brief リセット処理
  /// @retval true 書き込み成功
  /// @retval false 書き込み失敗
  bool reset() const noexcept;
  /// @brief ページ分のバッファをOLEDに書き込む
  /// @param[in] page ページ番号（0〜7）
  /// @retval true 書き込み成功
  /// @retval false 書き込み失敗
  bool sendBufferToDevice(uint8_t page) noexcept;
  /// @brief 全ページ分のバッファをOLEDに書き込む
  /// @retval true 書き込み成功
  /// @retval false 書き込み失敗
  bool sendBufferToDevice() noexcept;
  /// @brief エフェクトページをバッファに書き込む
  void drawEffectPage() noexcept;

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit SSD1306(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~SSD1306();
  /// @brief 通信可否
  /// @retval true 可
  /// @retval false 不可（デバイス繋がっていない等）
  bool ok() const noexcept override;
  /// @brief 表示対象のエフェクターを設定する<br>
  /// 画面全体の表示を全て更新する。<br>
  /// 選択中のパラメータは0にする。
  /// @param[in] src エフェクター設定
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool update(msg::OLED_DISP_EFFECTOR const &src) noexcept;
  /// @brief 選択中のエフェクトパラメータのカーソルを指定する
  /// @param[in] src 選択中のエフェクトパラメータ
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool update(msg::OLED_SELECT_PARAM const &src) noexcept;
  /// @brief エフェクトパラメータの表示更新（setParamCursorで指定したパラメータのみ更新する）
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool updateParam() noexcept;
  /// @brief バンク表示する
  /// @param[in] src バンク設定
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool update(msg::OLED_DISP_BANK const &src);
};