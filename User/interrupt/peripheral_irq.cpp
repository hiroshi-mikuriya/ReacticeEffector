/// @file      interrupt/peripheral_irq.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2022 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "main.h"

extern "C"
{
  /// @brief This function handles I2C1 event interrupt.
  void I2C1_EV_IRQHandler(void)
  {
    extern void i2cEvIRQ(void);
    i2cEvIRQ();
  }

  /// @brief This function handles I2C1 error interrupt.
  void I2C1_ER_IRQHandler(void)
  {
    extern void i2cErIRQ(void);
    i2cErIRQ();
  }

  /// @brief This function handles USART1 global interrupt.
  void USART1_IRQHandler(void)
  {
    extern void uart1RxIRQ(void);
    uart1RxIRQ();
  }

  /// @brief This function handles USART2 global interrupt.
  void USART2_IRQHandler(void)
  {
    extern void uart2RxIRQ(void);
    uart2RxIRQ();
  }

  /// @brief This function handles USB On The Go FS global interrupt.
  void OTG_FS_IRQHandler(void)
  {
    extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  }
}