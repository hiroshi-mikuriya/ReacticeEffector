/// @file      peripheral/spi_master.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"
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
  /// @brief コピーコンストラクタ削除
  SpiMaster(SpiMaster const &) = delete;
  /// @brief 代入演算子削除
  SpiMaster &operator=(SpiMaster const &) = delete;

  osThreadId threadId_; ///< 通信実行するスレッドID
  bool sendOnly_;       ///< 送信専用
  SPI_TypeDef *spi_;    ///< SPIペリフェラル
  DMA_TypeDef *dma_;    ///< DMA
  uint32_t txStream_;   ///< 送信DMAストリーム
  uint32_t rxStream_;   ///< 受信DMAストリーム

public:
  /// @brief 関数リターン値定義
  enum Result
  {
    OK = 0,  ///< 成功
    BUSY,    ///< ビジー
    TIMEOUT, ///< タイムアウト
    ERROR,   ///< エラー
  };
  /// @brief デフォルトコンストラクタ
  SpiMaster();
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
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  SpiMaster(SpiMaster &&that);
  /// @brief move演算子
  /// @param[in] that 移動元
  /// @return 自身の参照
  SpiMaster &operator=(SpiMaster &&that);
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
  /// @brief 送受信処理
  /// @param[in] tx 送信データの先頭ポインタ
  /// @param[in] rx 受信データの先頭ポインタ
  /// @param[in] size 通信データサイズ
  /// @param[in] millisec タイムアウト（ミリ秒）
  /// @retval OK      成功
  /// @retval BUSY    ビジー
  /// @retval TIMEOUT タイムアウト
  /// @retval ERROR   エラー
  Result sendRecv(uint8_t const *tx, uint8_t *rx, uint32_t size, uint32_t millisec = osWaitForever) noexcept;
  /// @brief 送信完了割り込みが発生したら呼び出す関数
  void notifyTxEndIRQ() noexcept;
  /// @brief 送信エラー割り込みが発生したら呼び出す関数
  void notifyTxErrorIRQ() noexcept;
  /// @brief 受信完了割り込みが発生したら呼び出す関数
  void notifyRxEndIRQ() noexcept;
  /// @brief 受信エラー割り込みが発生したら呼び出す関数
  void notifyRxErrorIRQ() noexcept;
};
