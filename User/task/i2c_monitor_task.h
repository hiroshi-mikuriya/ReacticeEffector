/// @file      task/i2c_monitor_task.h
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

  /// I2C Monitorタスクハンドル
  extern osThreadId i2cMonitorTaskHandle;

  /// @brief i2cMonitorTask内部処理
  /// @param[in] argument タスク引数
  void i2cMonitorTaskProc(void const *argument);

  /// @brief GPIO_IN_INT_SW_N 割り込み
  void notifyExtiSwIRQ(void);

  /// @brief GPIO_IN_INT_SW2_N 割り込み
  void notifyExtiSw2IRQ(void);

#ifdef __cplusplus
}
#endif
