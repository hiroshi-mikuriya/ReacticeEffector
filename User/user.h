/// @file      user.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "task/adc_task.h"
#include "task/app_task.h"
#include "task/i2c_task.h"
#include "task/neo_pixel_task.h"
#include "task/sound_task.h"
#include "task/usb_task.h"

#ifdef __cplusplus
#include <cstdint>
extern "C"
{
#else // C99
#include <stdint.h>
#endif

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

#ifdef __cplusplus
}
#endif
