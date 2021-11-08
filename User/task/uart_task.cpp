/// @file      task/uart_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "main.h"

extern "C"
{
  /// @brief UART1受信割り込み
  void uart1RxIRQ(void) { LL_USART_ReceiveData8(USART1); }
  /// @brief UART1 DMA送信完了割り込み
  void uart1TxEndIRQ(void) {}
  /// @brief UART1 DMA送信エラー割り込み
  void uart1TxErrorIRQ(void) {}
  /// @brief UART2受信割り込み
  void uart2RxIRQ(void) { LL_USART_ReceiveData8(USART2); }
  /// @brief UART2 DMA送信完了割り込み
  void uart2TxEndIRQ(void) {}
  /// @brief UART2 DMA送信エラー割り込み
  void uart2TxErrorIRQ(void) {}
} // extern "C"