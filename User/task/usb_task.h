/// @file      task/usb_task.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"
#include "stm32f7xx.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /// USB送信タスクハンドル
  extern osThreadId usbTxTaskHandle;

  /// @brief USB送信Task内部処理
  /// @param[in] argument タスク引数
  void usbTxTaskProc(void const *argument);

  /// @brief USB受信割り込み
  /// @param[in] bytes 受信データの先頭ポインタ
  /// @param[in] size 受信データサイズ
  void usbRxIRQ(uint8_t const *bytes, uint32_t size);

  /// @brief USB送信完了割り込み
  /// @param[in] bytes 受信データの先頭ポインタ
  /// @param[in] size 受信データサイズ
  void usbTxEndIRQ(uint8_t const *bytes, uint32_t size);

#ifdef __cplusplus
}
#endif
