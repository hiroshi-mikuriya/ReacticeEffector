/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/app_task.h"
#include "message/msglib.h"
#include "task/i2c_task.h"
#include "task/neo_pixel_task.h"
#include "task/usb_task.h"
#include <cstdlib> // rand

namespace
{
// TODO あとで消す
constexpr uint8_t V = 0x20;
constexpr satoh::msg::NEO_PIXEL_PATTERN RAINBOW = {{
    {V, 0, 0}, //
    {V, V, 0}, //
    {0, V, 0}, //
    {0, V, V}, //
    {0, 0, V}, //
    {V, 0, V}, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN BLUE = {{
    {0, 0, V / 2}, //
    {0, 0, V / 1}, //
    {0, 0, V * 2}, //
    {0, 0, V / 1}, //
    {0, 0, V / 2}, //
    {0, 0, V / 4}, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN GREEN = {{
    {0, V / 2, 0}, //
    {0, V / 1, 0}, //
    {0, V * 2, 0}, //
    {0, V / 1, 0}, //
    {0, V / 2, 0}, //
    {0, V / 4, 0}, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN RED = {{
    {V / 2, 0, 0}, //
    {V / 1, 0, 0}, //
    {V * 2, 0, 0}, //
    {V / 1, 0, 0}, //
    {V / 2, 0, 0}, //
    {V / 4, 0, 0}, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN PATTERNS[] = {RAINBOW, RED, GREEN, BLUE};
} // namespace

void appTaskProc(void const *argument)
{
  satoh::addMsgTarget(4);
  satoh::msg::NEO_PIXEL_SPEED speed = {100};
  for (;;)
  {
    auto res = satoh::recvMsg();
    if (res.status() != osOK)
    {
      continue;
    }
    auto *msg = res.msg();
    if (msg == 0)
    {
      continue;
    }
    switch (msg->type)
    {
    case satoh::msg::MODE_KEY_NOTIFY:
    {
      // TODO
      break;
    }
    case satoh::msg::EFFECT_KEY_CHANGED_NOTIFY:
    {
      constexpr uint8_t V = 0x80;
      constexpr satoh::RGB COLORS[] = {
          {V, 0, 0}, //
          {V, V, 0}, //
          {0, V, 0}, //
          {0, V, V}, //
          {0, 0, V}, //
          {V, 0, V}, //
      };
      auto *param = reinterpret_cast<satoh::msg::EFFECT_KEY const *>(msg->bytes);
      for (int i = 0; i < 4; ++i)
      {
        if (param->button[i])
        {
          satoh::msg::LED_EFFECT src{};
          src.led = i;
          src.rgb = COLORS[rand() % 6];
          satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_EFFECT_REQ, &src, sizeof(src));
          auto ptn = PATTERNS[i];
          satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &ptn, sizeof(ptn));
        }
      }
      break;
    }
    case satoh::msg::GYRO_NOTIFY:
      satoh::sendMsg(usbTxTaskHandle, satoh::msg::USB_TX_REQ, msg->bytes, msg->size);
      break;
    case satoh::msg::ROTARY_ENCODER_NOTIFY:
    {
      auto *param = reinterpret_cast<satoh::msg::ROTARY_ENCODER const *>(msg->bytes);
      // POWER LED, MODULATION LED
      for (uint8_t i = 0; i < 2; ++i)
      {
        if (param->angleDiff[0] != 0)
        {
          satoh::msg::LED_SIMPLE src{};
          src.led = i;
          src.level = 0 < param->angleDiff[0];
          satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_SIMPLE_REQ, &src, sizeof(src));
        }
      }
      // NeoPixel Speed
      if (param->angleDiff[1] != 0)
      {
        if (0 < param->angleDiff[1])
        {
          speed.interval -= 10;
        }
        else
        {
          speed.interval += 10;
        }
        if (speed.interval <= 0)
        {
          speed.interval = 10;
        }
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_SPEED, &speed, sizeof(speed));
      }
      break;
    }
    }
  }
}