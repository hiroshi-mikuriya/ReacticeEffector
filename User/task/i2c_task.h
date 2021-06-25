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
  /// @param[in] i2c I2Cペリフェラル
  void i2cEvIRQ(I2C_TypeDef *i2c);

  /// @brief I2Cエラー割り込み
  /// @param[in] i2c I2Cペリフェラル
  void i2cErIRQ(I2C_TypeDef *i2c);

  /// @brief GPIO_IN_INT_MPU_N 割り込み
  void extiMpuIRQ(void);

  /// @brief GPIO_IN_INT_SW_N 割り込み
  void extiSwIRQ(void);

  /// @brief GPIO_IN_INT_SW2_N 割り込み
  void extiSw2IRQ(void);

#ifdef __cplusplus
}
#endif
