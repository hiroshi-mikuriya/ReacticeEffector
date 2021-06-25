/// @file      task/neo_pixel_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "neo_pixel_task.h"
#include "cmsis_os.h"
#include "device/neo_pixel.h"

/// 送信完了通知
constexpr int32_t NEO_PIXEL_TX_END = 1 << 0;
/// 送信エラー通知
constexpr int32_t NEO_PIXEL_TX_ERROR = 1 << 1;

void nepPixelTaskImpl(void const *argument)
{
  LL_GPIO_SetOutputPin(LED_ACT_GPIO_Port, LED_ACT_Pin);
  constexpr uint32_t LED_COUNT = 100;
  constexpr uint8_t V = 0x20;
  satoh::NeoPixel np(SPI3, DMA1, LL_DMA_STREAM_5, LED_COUNT);
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

void neoPixelTxEndIRQ(void)
{
  extern osThreadId neoPixelTaskHandle;
  osSignalSet(neoPixelTaskHandle, NEO_PIXEL_TX_END);
}

void neoPixelTxErrorIRQ(void)
{
  extern osThreadId neoPixelTaskHandle;
  osSignalSet(neoPixelTaskHandle, NEO_PIXEL_TX_ERROR);
}
