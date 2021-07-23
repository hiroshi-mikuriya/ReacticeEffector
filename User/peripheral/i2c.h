/// @file      peripheral/i2c.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"
#include "common/alloc.hpp"
#include "stm32f7xx.h"

namespace satoh
{
class I2C;
/// I2Cクラス専用シグナルマスク
constexpr int32_t I2C_CLS_SIG_MASK = 0x0000FFFF;
} // namespace satoh

/// @brief I2C通信クラス
class satoh::I2C
{
  I2C_TypeDef *const i2cx_;      ///< I2Cペリフェラル
  osThreadId threadId_;          ///< イベント通知先のスレッドID
  DMA_TypeDef *const dma_;       ///< 送受信DMA
  const uint32_t rxStream_;      ///< 受信DMAストリーム
  const uint32_t txStream_;      ///< 送信DMAストリーム
  mutable Alloc<uint8_t> rxbuf_; ///< 受信バッファ
  mutable Alloc<uint8_t> txbuf_; ///< 送信バッファ

  /// @brief 初期化
  void start() const noexcept;
  /// @brief 終了処理
  void stop() const noexcept;
  /// @brief 再初期化
  void restart() const noexcept;

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
  /// @param[in] dma 送受信DMA
  /// @param[in] rxStream 受信DMAストリーム
  /// @param[in] txStream 送信DMAストリーム
  explicit I2C(I2C_TypeDef *const i2cx, osThreadId threadId, DMA_TypeDef *const dma, uint32_t rxStream, uint32_t txStream) noexcept;
  /// @brief デストラクタ
  virtual ~I2C();
  /// @brief イベント割り込みが発生したら呼び出す関数
  void notifyEvIRQ() noexcept;
  /// @brief エラー割り込みが発生したら呼び出す関数
  void notifyErIRQ() noexcept;
  /// @brief 受信完了割り込みが発生したら呼び出す関数
  void notifyRxEndIRQ() noexcept;
  /// @brief 受信エラー割り込みが発生したら呼び出す関数
  void notifyRxErrorIRQ() noexcept;
  /// @brief 送信完了割り込みが発生したら呼び出す関数
  void notifyTxEndIRQ() noexcept;
  /// @brief 送信エラー割り込みが発生したら呼び出す関数
  void notifyTxErrorIRQ() noexcept;
  /// @brief データを書き込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] bytes 書き込みデータの先頭ポインタ
  /// @param[in] size 書き込むバイト数
  /// @param[in] withSleep 通信完了後の1ミリ秒スリープ有無
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval NACK    NACK
  /// @retval ERROR   エラー
  Result write(uint8_t slaveAddr, uint8_t const *bytes, uint32_t size, bool withSleep) const noexcept;
  /// @brief データを読み込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] buf 読み込んだデータを格納するバッファ
  /// @param[in] size 読み込むバイト数
  /// @param[in] withSleep 通信完了後の1ミリ秒スリープ有無
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval NACK    NACK
  /// @retval ERROR   エラー
  Result read(uint8_t slaveAddr, uint8_t *buffer, uint32_t size, bool withSleep) const noexcept;
};