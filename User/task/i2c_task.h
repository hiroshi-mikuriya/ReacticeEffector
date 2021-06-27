/// @file      task/i2c_task.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "stm32f7xx.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /// @brief i2cTask内部処理
  /// @param[in] argument タスク引数
  void i2cTaskImpl(void const *argument);

  /// @brief I2Cイベント割り込み
  void i2cEvIRQ();

  /// @brief I2Cエラー割り込み
  void i2cErIRQ();

  /// @brief I2C送信完了割り込み
  void i2cTxEndIRQ();

  /// @brief I2C送信エラー割り込み
  void i2cTxErrorIRQ();

  /// @brief GPIO_IN_INT_MPU_N 割り込み
  void extiMpuIRQ(void);

  /// @brief GPIO_IN_INT_SW_N 割り込み
  void extiSwIRQ(void);

  /// @brief GPIO_IN_INT_SW2_N 割り込み
  void extiSw2IRQ(void);

#ifdef __cplusplus
}
#endif
