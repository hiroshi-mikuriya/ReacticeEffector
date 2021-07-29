/// @file      device/at42qt1070.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c_device_base.hpp"

namespace satoh
{
class AT42QT1070;
} // namespace satoh

class satoh::AT42QT1070 : public satoh::I2CDeviceBase
{
  /// デフォルトコンストラクタ削除
  AT42QT1070() = delete;
  /// コピーコンストラクタ削除
  AT42QT1070(AT42QT1070 const &) = delete;
  /// 代入演算子削除
  AT42QT1070 &operator=(AT42QT1070 const &) = delete;
  /// moveコンストラクタ削除
  AT42QT1070(AT42QT1070 &&) = delete;
  /// move演算子削除
  AT42QT1070 &operator=(AT42QT1070 &&) = delete;
  /// 通信可否
  const bool ok_;
  /// 前回測定値
  mutable uint8_t cache_;
  /// @brief センサ初期化 @retval true 成功 @retval false 失敗
  bool init() const noexcept;

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit AT42QT1070(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~AT42QT1070() {}
  /// @brief デバイス状態を取得する @retval true 正常 @retval false 異常あり
  explicit operator bool() const noexcept override { return ok_; };
  /// @brief キー状態取得
  /// @param[out] keys キー状態格納先
  ///   @arg 0 TAP
  ///   @arg 1 RE1
  ///   @arg 2 DOWN
  ///   @arg 3 MODE
  ///   @arg 4 UP
  ///   @arg 5 RETURN
  /// @retval -1 通信失敗
  /// @retval 0 変化なし
  /// @retval 1 変化あり
  int read(uint8_t (&keys)[6]) const noexcept;
};
