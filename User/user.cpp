/// @file      user.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "user.h"
#include "usbd_cdc_if.h"

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
