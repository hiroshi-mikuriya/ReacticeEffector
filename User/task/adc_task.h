/// @file      task/adc_task.h
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

  /// ADCタスクハンドル
  extern osThreadId adcTaskHandle;

  /// @brief adcTask内部処理
  /// @param[in] argument タスク引数
  void adcTaskProc(void const *argument);

  /// @brief ADC変換完了DMA割り込み
  void adc1CpltIRQ(void);

  /// @brief ADC変換エラーDMA割り込み
  void adc1ErrorIRQ(void);

  /// @brief ADC TIM割り込み
  void adc1TimIRQ(void);

#ifdef __cplusplus
}
#endif
