/// @file      device/pca9635.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include "i2c_device_base.hpp"

namespace satoh
{
class PCA9635;
}

/// @brief PCA9635制御クラス（エフェクトLED）
class satoh::PCA9635 : public satoh::I2CDeviceBase
{
  /// デフォルトコンストラクタ削除
  PCA9635() = delete;
  /// コピーコンストラクタ削除
  PCA9635(PCA9635 const &) = delete;
  /// 代入演算子削除
  PCA9635 &operator=(PCA9635 const &) = delete;
  /// moveコンストラクタ削除
  PCA9635(PCA9635 &&) = delete;
  /// move演算子削除
  PCA9635 &operator=(PCA9635 &&) = delete;
  /// 通信可否
  const bool ok_;
  /// @brief I2C書き込み
  /// @param [in] reg 書き込み先のレジスタ
  /// @param [in] bytes 書き込みデータ
  /// @param [in] size 書き込みサイズ
  bool write(uint8_t reg, void const *bytes, uint32_t size) const noexcept;
  /// @brief デバイス初期化
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool init() const noexcept;

public:
  /// @brief コンストラクタ
  /// @param [in] i2c I2C通信オブジェクト
  explicit PCA9635(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~PCA9635() {}
  /// @brief デバイス状態を取得する @retval true 正常 @retval false 異常あり
  explicit operator bool() const noexcept override { return ok_; };
  /// @brief LEDの色値を設定
  /// @param [in] rgb 色値
  /// @param [in] n LED番号（0 - 3）
  /// @retval true 設定成功
  /// @retval false I2C通信失敗、LED番号不正等
  bool set(RGB const &rgb, uint8_t n) const noexcept;
  /// @brief 全LEDの色値を設定
  /// @param [in] rgb 色値
  /// @retval true 設定成功
  /// @retval false I2C通信失敗、LED番号不正等
  bool set(RGB const (&rgb)[4]) const noexcept;
};