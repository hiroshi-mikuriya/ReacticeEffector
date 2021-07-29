/// @file      device/rotary_encoder.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "i2c_device_base.hpp"

namespace satoh
{
class RotaryEncoder;
}

/// @brief ロータリーエンコーダ制御クラス
class satoh::RotaryEncoder : public satoh::I2CDeviceBase
{
  /// デフォルトコンストラクタ削除
  RotaryEncoder() = delete;
  /// コピーコンストラクタ削除
  RotaryEncoder(RotaryEncoder const &) = delete;
  /// 代入演算子削除
  RotaryEncoder &operator=(RotaryEncoder const &) = delete;
  /// moveコンストラクタ削除
  RotaryEncoder(RotaryEncoder &&) = delete;
  /// move演算子削除
  RotaryEncoder &operator=(RotaryEncoder &&) = delete;
  /// デバイス状態
  const bool ok_;
  /// キャッシュ（前回値）
  uint8_t cache_[2];

  /// @brief デバイス初期化
  /// @retval true 通信成功
  /// @retval false 通信失敗
  bool init() noexcept;

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit RotaryEncoder(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~RotaryEncoder() {}
  /// @brief デバイス状態を取得する @retval true 正常 @retval false 異常あり
  explicit operator bool() const noexcept override { return ok_; };
  /// @brief デバイスからレジスタを読み出す
  /// @param[out] button ボタン状態
  ///   @arg BUTTON_UP ボタン離し中
  ///   @arg BUTTON_DOWN ボタン押下中
  /// @param[out] angleDiff 角度変化量
  /// @retval -1 通信失敗
  /// @retval 0 前回から変更なし
  /// @retval 1 ロータリーエンコーダ状態に変化あり
  /// @retval 2 ボタン押下状態に変化あり
  /// @retval 3 ロータリーエンコーダ、ボタン押下状態の両方に変化あり
  int read(uint8_t (&button)[4], int8_t (&angleDiff)[4]) noexcept;
};