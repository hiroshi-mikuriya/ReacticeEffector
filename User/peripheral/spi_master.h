/// @file      peripheral/spi_master.h
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
class SpiMaster;
/// SPIマスタークラス専用シグナルマスク
constexpr int32_t SPI_MASTER_CLS_SIG_MASK = 0x000000FF;
} // namespace satoh

/// @brief SPIマスター通信クラス
class satoh::SpiMaster
{
  osThreadId threadId_;       ///< 通信実行するスレッドID
  bool sendOnly_;             ///< 送信専用
  SPI_TypeDef *const spi_;    ///< SPIペリフェラル
  DMA_TypeDef *const dma_;    ///< DMA
  const uint32_t txStream_;   ///< 送信DMAストリーム
  unique_ptr<uint8_t> txbuf_; ///< DMA送信バッファ
  const uint32_t rxStream_;   ///< 受信DMAストリーム
  unique_ptr<uint8_t> rxbuf_; ///< DMA受信バッファ

public:
  /// @brief 関数リターン値定義
  enum Result
  {
    OK = 0,  ///< 成功
    BUSY,    ///< ビジー
    TIMEOUT, ///< タイムアウト
    ERROR,   ///< エラー
  };
  /// @brief コンストラクタ（送信専用初期化）
  /// @param[in] threadId 通信実行するスレッドID
  /// @param[in] spi SPIペリフェラル
  /// @param[in] dma DMA
  /// @param[in] txStream 送信DMAストリーム
  /// @param[in] bufferSize 最大バッファサイズ
  explicit SpiMaster(osThreadId threadId, SPI_TypeDef *spi, DMA_TypeDef *dma, uint32_t txStream, uint32_t bufferSize) noexcept;
  /// @brief コンストラクタ（送受信用初期化）
  /// @param[in] threadId 通信実行するスレッドID
  /// @param[in] spi SPIペリフェラル
  /// @param[in] dma DMA
  /// @param[in] txStream 送信DMAストリーム
  /// @param[in] rxStream 受信DMAストリーム
  /// @param[in] bufferSize 最大バッファサイズ
  explicit SpiMaster(osThreadId threadId, SPI_TypeDef *spi, DMA_TypeDef *dma, uint32_t txStream, uint32_t rxStream, uint32_t bufferSize) noexcept;
  /// @brief デストラクタ
  virtual ~SpiMaster();
  /// @brief 送信処理
  /// @param[in] bytes 送信データの先頭ポインタ
  /// @param[in] size 送信データサイズ
  /// @param[in] millisec タイムアウト（ミリ秒）
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval ERROR   エラー
  Result send(uint8_t const *bytes, uint32_t size, uint32_t millisec = osWaitForever) noexcept;
  /// @brief 送信完了割り込みが発生したら呼び出す関数
  void notifyTxEndIRQ() noexcept;
  /// @brief 送信エラー割り込みが発生したら呼び出す関数
  void notifyTxErrorIRQ() noexcept;
};
