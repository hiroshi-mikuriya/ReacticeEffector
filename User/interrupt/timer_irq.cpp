/// @file      interrupt/timer_irq.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2022 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "main.h"

extern "C"
{
  /// @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  void TIM1_UP_TIM10_IRQHandler(void)
  {
    if (LL_TIM_IsActiveFlag_UPDATE(TIM10))
    {
      LL_TIM_ClearFlag_UPDATE(TIM10);
      extern void adc1TimIRQ(void);
      extern void appTimIRQ(void);
      adc1TimIRQ();
      appTimIRQ();
    }
  }

  /// @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  void TIM6_DAC_IRQHandler(void)
  {
    if (LL_TIM_IsActiveFlag_UPDATE(TIM6))
    {
      LL_TIM_ClearFlag_UPDATE(TIM6);
      LL_GPIO_TogglePin(LED_ACT_GPIO_Port, LED_ACT_Pin);
    }
  }

  /// @brief This function handles TIM7 global interrupt.
  void TIM7_IRQHandler(void)
  {
    extern TIM_HandleTypeDef htim7;
    HAL_TIM_IRQHandler(&htim7);
  }
}