/// @file      user.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "user.h"
#include "cmsis_os.h"
#include "main.h"
#include "neo_pixel.h"
#include "usbd_cdc_if.h"

constexpr int32_t NEO_PIXEL_TX_END = 1 << 0;
constexpr int32_t NEO_PIXEL_TX_ERROR = 1 << 1;

void StartDefaultTaskImpl(void const *argument)
{
  LL_GPIO_SetOutputPin(LED_ACT_GPIO_Port, LED_ACT_Pin);
  constexpr uint32_t LED_COUNT = 100;
  constexpr uint8_t V = 0x20;
  sato::NeoPixel np(SPI3, DMA1, LL_DMA_STREAM_5, LED_COUNT);
  for (;;)
  {
    for (int i = 0; i < 6; ++i)
    {
      np.clear();
      for (uint32_t n = 0; n < LED_COUNT; ++n)
      {
        switch ((n + i) % 6)
        {
        case 0:
          np.set({V, 0, 0}, n);
          break;
        case 1:
          np.set({V, V, 0}, n);
          break;
        case 2:
          np.set({0, V, 0}, n);
          break;
        case 3:
          np.set({0, V, V}, n);
          break;
        case 4:
          np.set({0, 0, V}, n);
          break;
        case 5:
          np.set({V, 0, V}, n);
          break;
        }
      }
      np.show();
      osSignalWait(NEO_PIXEL_TX_END | NEO_PIXEL_TX_ERROR, osWaitForever);
      osDelay(100);
    }
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
  extern osThreadId defaultTaskHandle;
  osSignalSet(defaultTaskHandle, NEO_PIXEL_TX_END);
}

void neoPixelTxErrorIRQ(void)
{
  extern osThreadId defaultTaskHandle;
  osSignalSet(defaultTaskHandle, NEO_PIXEL_TX_ERROR);
}
