/// @file      task/app_tasl.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /// Appタスクハンドル
  extern osThreadId appTaskHandle;

  /// @brief appTask内部処理
  /// @param [in] argument タスク引数
  void appTaskProc(void const *argument);

  /// @brief APP TIM割り込み
  void appTimIRQ(void);

  /// @brief SPI SRAM送信完了割り込み
  void spiSramTxEndIRQ(void);

  /// @brief SPI SRAM送信エラー割り込み
  void spiSramTxErrorIRQ(void);

  /// @brief SPI SRAM受信完了割り込み
  void spiSramRxEndIRQ(void);

  /// @brief SPI SRAM受信エラー割り込み
  void spiSramRxErrorIRQ(void);

#ifdef __cplusplus
}
#endif
