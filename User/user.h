/// @file      user.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#ifdef __cplusplus
#include <cstdint>
extern "C"
{
#else // C99
#include <stdint.h>
#endif

  /// @brief StartDefaultTask内部処理
  /// @param[in] argument タスク引数
  void StartDefaultTaskImpl(void const *argument);

  /// @brief USB受信割り込み
  /// @param[in] bytes 受信データの先頭ポインタ
  /// @param[in] size 受信データサイズ
  void usbRxIRQ(uint8_t const *bytes, uint32_t size);

  /// @brief GPIO_IN_INT_SW_N 割り込み
  void extiSwIRQ(void);

  /// @brief GPIO_IN_INT_SW2_N 割り込み
  void extiSw2IRQ(void);

  /// @brief GPIO_IN_INT_MPU_N 割り込み
  void extiMpuIRQ(void);

  /// @brief UART1受信割り込み
  void uart1RxIRQ(void);

  /// @brief UART1 DMA送信完了割り込み
  void uart1TxEndIRQ(void);

  /// @brief UART1 DMA送信エラー割り込み
  void uart1TxErrorIRQ(void);

  /// @brief UART2受信割り込み
  void uart2RxIRQ(void);

  /// @brief UART2 DMA送信完了割り込み
  void uart2TxEndIRQ(void);

  /// @brief UART2 DMA送信エラー割り込み
  void uart2TxErrorIRQ(void);

  /// @brief NeoPixel DMA送信完了割り込み
  void neoPixelTxEndIRQ(void);

  /// @brief NeoPixel DMA送信エラー割り込み
  void neoPixelTxErrorIRQ(void);

#ifdef __cplusplus
}
#endif
