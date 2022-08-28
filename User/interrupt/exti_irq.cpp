/// @file      interrupt/timer_irq.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2022 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "main.h"

extern "C"
{
  /// @brief This function handles EXTI line[0] interrupts.
  void EXTI0_IRQHandler(void)
  {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
      // do nothing
    }
  }

  /// @brief This function handles EXTI line[1] interrupts.
  void EXTI1_IRQHandler(void)
  {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
      // do nothing
    }
  }

  /// @brief This function handles EXTI line[2] interrupts.
  void EXTI2_IRQHandler(void)
  {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
      // do nothing
    }
  }

  /// @brief This function handles EXTI line[3] interrupts.
  void EXTI3_IRQHandler(void)
  {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
      // do nothing
    }
  }

  /// @brief This function handles EXTI line[4] interrupts.
  void EXTI4_IRQHandler(void)
  {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
      // do nothing
    }
  }

  /// @brief This function handles EXTI line[9:5] interrupts.
  void EXTI9_5_IRQHandler(void)
  {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
      extern void extiSw2IRQ(void);
      extiSw2IRQ();
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
      extern void extiMpuIRQ(void);
      extiMpuIRQ();
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_7))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_7);
      extern void extiSwIRQ(void);
      extiSwIRQ();
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_8))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
      // do nothing
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_9))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
      // do nothing
    }
  }

  /// @brief This function handles EXTI line[15:10] interrupts.
  void EXTI15_10_IRQHandler(void)
  {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_10))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
      // do nothing
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_11))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
      // TODO RPI SPI NSS
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_12))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_12);
      // do nothing
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
      // do nothing
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_14))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_14);
      // do nothing
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_15))
    {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);
      // do nothing
    }
  }
}