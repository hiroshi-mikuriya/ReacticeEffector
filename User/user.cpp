/// @file      user.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "user.h"
#include "cmsis_os.h"
#include "main.h"
#include "usbd_cdc_if.h"

void StartDefaultTaskImpl(void const *argument)
{
  for (;;)
  {
    LL_GPIO_TogglePin(LED_ACT_GPIO_Port, LED_ACT_Pin);
    osDelay(500);
  }
}

void usbRxIRQ(uint8_t const *bytes, uint32_t size)
{
  // TODO
  CDC_Transmit_FS(const_cast<uint8_t *>(bytes), static_cast<uint16_t>(size));
}

void extiSwIRQ(void)
{
  // TODO
}

void extiSw2IRQ(void)
{
  // TODO
}

void extiMpuIRQ(void)
{
  // TODO
}

void uart1RxIRQ(void)
{
  // TODO
  LL_USART_ReceiveData8(USART1);
}

void uart1TxEndIRQ(void)
{
  // TODO
}

void uart1TxErrorIRQ(void)
{
  // TODO
}

void uart2RxIRQ(void)
{
  // TODO
  LL_USART_ReceiveData8(USART2);
}

void uart2TxEndIRQ(void)
{
  // TODO
}

void uart2TxErrorIRQ(void)
{
  // TODO
}

void neoPixelTxEndIRQ(void)
{
  // TODO
}

void neoPixelTxErrorIRQ(void)
{
  // TODO
}
