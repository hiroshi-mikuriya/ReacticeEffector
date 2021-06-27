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
  I2C_TypeDef *const i2cx_; ///< I2Cペリフェラル
  osThreadId threadId_;     ///< イベント通知先のスレッドID
  DMA_TypeDef *const dma_;  ///< 送信DMA
  const uint32_t stream_;   ///< 送信DMAストリーム
  uint8_t *rxbuf_;          ///< 読み込みバッファ

  void start();
  void stop();
  /// @brief 再初期化
  void restart();

public:
  /// @brief 関数リターン値定義
  enum Result
  {
    OK = 0,  ///< 成功
    BUSY,    ///< ビジー
    TIMEOUT, ///< タイムアウト
    NACK,    ///< NACK
    ERROR,   ///< エラー
  };
  /// @brief コンストラクタ
  /// @param[in] i2cx I2Cペリフェラル
  /// @param[in] threadId イベント通知先のスレッドID
  /// @param[in] dma 送信DMA
  /// @param[in] stream 送信DMAストリーム
  explicit I2C(I2C_TypeDef *const i2cx, osThreadId threadId, DMA_TypeDef *const dma, uint32_t stream) noexcept;
  /// @brief デストラクタ
  virtual ~I2C();
  /// @brief イベント割り込みが発生したら呼び出す関数
  void notifyEvIRQ();
  /// @brief エラー割り込みが発生したら呼び出す関数
  void notifyErIRQ();
  /// @brief 送信完了割り込みが発生したら呼び出す関数
  void notifyTxEndIRQ();
  /// @brief 送信エラー割り込みが発生したら呼び出す関数
  void notifyTxErrorIRQ();
  /// @brief データを書き込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] bytes 書き込みデータの先頭ポインタ
  /// @param[in] size 書き込むバイト数
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval NACK    NACK
  /// @retval ERROR   エラー
  Result write(uint8_t slaveAddr, uint8_t const *bytes, uint32_t size) noexcept;
  /// @brief データを読み込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] buf 読み込んだデータを格納するバッファ
  /// @param[in] size 読み込むバイト数
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval NACK    NACK
  /// @retval ERROR   エラー
  Result read(uint8_t slaveAddr, uint8_t *buffer, uint32_t size) noexcept;
};