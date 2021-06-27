/// @file      device/i2c.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"
#include "stm32f7xx.h"

namespace satoh
{
class I2C;
} // namespace satoh

/// @brief I2C通信クラス
class satoh::I2C
{
  /// I2Cペリフェラル
  I2C_TypeDef *const i2cx_;
  /// イベント通知先のスレッドID
  osThreadId threadId_;
  /// 読み込みバッファ
  uint8_t *rxbuf_;
  /// 書き込みバッファ
  uint8_t const *txbuf_;

public:
  /// @brief 関数リターン値定義
  enum Result
  {
    OK = 0,  ///< 成功
    BUSY,    ///< ビジー
    TIMEOUT, ///< タイムアウト
    NACK,    ///< NACK
  };
  /// @brief コンストラクタ
  /// @param[in] i2cx I2Cペリフェラル
  /// @param[in] threadId イベント通知先のスレッドID
  explicit I2C(I2C_TypeDef *const i2cx, osThreadId threadId) noexcept;
  /// @brief デストラクタ
  virtual ~I2C();
  /// @brief イベント割り込みが発生したら呼び出す関数
  void notifyEvIRQ();
  /// @brief エラー割り込みが発生したら呼び出す関数
  void notifyErIRQ();
  /// @brief データを書き込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] bytes 書き込みデータの先頭ポインタ
  /// @param[in] size 書き込むバイト数
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval NACK    NACK
  Result write(uint8_t slaveAddr, uint8_t const *bytes, uint32_t size) noexcept;
  /// @brief データを読み込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] buf 読み込んだデータを格納するバッファ
  /// @param[in] size 読み込むバイト数
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval NACK    NACK
  Result read(uint8_t slaveAddr, uint8_t *buffer, uint32_t size) noexcept;
};