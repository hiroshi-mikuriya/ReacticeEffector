/// @file      task/i2c_task.h
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

  /// I2Cタスクハンドル
  extern osThreadId i2cTaskHandle;

  /// @brief i2cTask内部処理
  /// @param [in] argument タスク引数
  void i2cTaskProc(void const *argument);

  /// @brief I2Cイベント割り込み
  void i2cEvIRQ(void);

  /// @brief I2Cエラー割り込み
  void i2cErIRQ(void);

  /// @brief I2C受信完了割り込み
  void i2cRxEndIRQ(void);

  /// @brief I2C受信エラー割り込み
  void i2cRxErrorIRQ(void);

  /// @brief I2C送信完了割り込み
  void i2cTxEndIRQ(void);

  /// @brief I2C送信エラー割り込み
  void i2cTxErrorIRQ(void);

  /// @brief GPIO_IN_INT_MPU_N 割り込み
  void extiMpuIRQ(void);

  /// @brief GPIO_IN_INT_SW_N 割り込み
  void extiSwIRQ(void);

  /// @brief GPIO_IN_INT_SW2_N 割り込み
  void extiSw2IRQ(void);

#ifdef __cplusplus
}
#endif
