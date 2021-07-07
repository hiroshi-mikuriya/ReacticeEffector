/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/app_task.h"
#include "message/msglib.h"
#include "task/i2c_task.h"
#include "task/neo_pixel_task.h"
#include "task/sound_task.h"
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
    {0, 0, V * 1}, //
    {0, 0, V * 2}, //
    {0, 0, V * 4}, //
    {0, 0, V * 2}, //
    {0, 0, V * 1}, //
    {0, 0, V / 2}, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN GREEN = {{
    {0, V * 1, 0}, //
    {0, V * 2, 0}, //
    {0, V * 4, 0}, //
    {0, V * 2, 0}, //
    {0, V * 1, 0}, //
    {0, V / 2, 0}, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN RED = {{
    {V * 1, 0, 0}, //
    {V * 2, 0, 0}, //
    {V * 4, 0, 0}, //
    {V * 2, 0, 0}, //
    {V * 1, 0, 0}, //
    {V / 2, 0, 0}, //
}};
} // namespace

void appTaskProc(void const *argument)
{
  satoh::addMsgTarget(4);
  satoh::msg::NEO_PIXEL_SPEED speed = {100};
  satoh::msg::LED_SIMPLE power = {0, false};
  satoh::msg::LED_SIMPLE modulation = {1, false};
  for (int n = 0;;)
  {
    auto res = satoh::recvMsg();
    auto *msg = res.msg();
    if (msg == 0)
    {
      continue;
    }
    switch (msg->type)
    {
    case satoh::msg::MODE_KEY_NOTIFY:
    {
      auto *param = reinterpret_cast<satoh::msg::MODE_KEY const *>(msg->bytes);
      if (param->up == satoh::msg::BUTTON_DOWN)
      {
        n = (n + 1) % 6;
        satoh::msg::LED_EFFECT led{0, RAINBOW.rgb[n]};
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_EFFECT_REQ, &led, sizeof(led));
      }
      if (param->ok == satoh::msg::BUTTON_DOWN)
      {
        n = (n + 1) % 6;
        satoh::msg::LED_EFFECT led{1, RAINBOW.rgb[n]};
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_EFFECT_REQ, &led, sizeof(led));
      }
      if (param->rtn == satoh::msg::BUTTON_DOWN)
      {
        n = (n + 1) % 6;
        satoh::msg::LED_EFFECT led{2, RAINBOW.rgb[n]};
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_EFFECT_REQ, &led, sizeof(led));
      }
      if (param->down == satoh::msg::BUTTON_DOWN)
      {
        n = (n + 1) % 6;
        satoh::msg::LED_EFFECT led{3, RAINBOW.rgb[n]};
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_EFFECT_REQ, &led, sizeof(led));
      }
      if (param->tap == satoh::msg::BUTTON_DOWN)
      {
        modulation.level = !modulation.level;
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_SIMPLE_REQ, &modulation, sizeof(modulation));
      }
      if (param->re1 == satoh::msg::BUTTON_DOWN)
      {
        power.level = !power.level;
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_SIMPLE_REQ, &power, sizeof(power));
      }
      break;
    }
    case satoh::msg::EFFECT_KEY_CHANGED_NOTIFY:
    {
      auto *param = reinterpret_cast<satoh::msg::EFFECT_KEY const *>(msg->bytes);
      if (param->button[0] == satoh::msg::BUTTON_DOWN)
      {
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &RAINBOW, sizeof(RAINBOW));
      }
      if (param->button[1] == satoh::msg::BUTTON_DOWN)
      {
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &RED, sizeof(RED));
      }
      if (param->button[2] == satoh::msg::BUTTON_DOWN)
      {
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &GREEN, sizeof(GREEN));
      }
      if (param->button[3] == satoh::msg::BUTTON_DOWN)
      {
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &BLUE, sizeof(BLUE));
      }
      break;
    }
    case satoh::msg::GYRO_NOTIFY:
      satoh::sendMsg(usbTxTaskHandle, satoh::msg::USB_TX_REQ, msg->bytes, msg->size);
      break;
    case satoh::msg::ROTARY_ENCODER_NOTIFY:
    {
      satoh::sendMsg(soundTaskHandle, msg->type, msg->bytes, msg->size); // TODO 仮
      auto *param = reinterpret_cast<satoh::msg::ROTARY_ENCODER const *>(msg->bytes);
      for (uint8_t i = 0; i < 4; ++i)
      {
        if (0 < param->angleDiff[i] && 10 < speed.interval)
        {
          speed.interval -= 10;
        }
        if (param->angleDiff[i] < 0 && speed.interval <= 300)
        {
          speed.interval += 10;
        }
      }
      satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_SPEED, &speed, sizeof(speed));
      break;
    }
    }
  }
}