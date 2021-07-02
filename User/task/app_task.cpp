/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "app_task.h"
#include "i2c_task.h"
#include "message/msglib.h"
#include <cstdlib> // rand

void appTaskProc(void const *argument)
{
  constexpr uint32_t DELAY = 20;
  for (int i = 0;; i = (i + 1) % 8)
  {
    {
      constexpr uint8_t V = 0x80;
      constexpr satoh::RGB COLORS[] = {
          {V, 0, 0}, //
          {V, V, 0}, //
          {0, V, 0}, //
          {0, V, V}, //
          {0, 0, V}, //
          {V, 0, V}, //
          {V, V, V}, //
          {0, 0, 0}, //
      };
      satoh::msg::LED_EFFECT src{};
      src.led = i % 4;
      src.rgb = COLORS[rand() % 8];
      satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_EFFECT_REQ, &src, sizeof(src));
    }
    osDelay(DELAY);
    {
      satoh::msg::LED_SIMPLE src{};
      src.led = i % 2;
      src.level = !!(i % 3);
      satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_SIMPLE_REQ, &src, sizeof(src));
    }
    osDelay(DELAY);
    {
      satoh::msg::LED_LEVEL src{};
      src.left = 7 - i;
      src.right = i;
      satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_LEVEL_UPDATE_REQ, &src, sizeof(src));
    }
    osDelay(DELAY);
  }
}