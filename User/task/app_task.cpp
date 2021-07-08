/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/app_task.h"
#include "effector/chorus.hpp"
#include "effector/distortion.hpp"
#include "effector/overdrive.hpp"
#include "effector/tremolo.hpp"
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
constexpr satoh::RGB RED = {V, 0, 0};
constexpr satoh::RGB YELLOW = {V, V, 0};
constexpr satoh::RGB GREEN = {0, V, 0};
constexpr satoh::RGB AQUA = {0, V, V};
constexpr satoh::RGB BLUE = {0, 0, V};
constexpr satoh::RGB PINK = {V, 0, V};
constexpr satoh::msg::NEO_PIXEL_PATTERN RAINBOW_PTN = {{
    RED,    //
    YELLOW, //
    GREEN,  //
    AQUA,   //
    BLUE,   //
    PINK,   //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN BLUE_PTN = {{
    BLUE * 1, //
    BLUE * 2, //
    BLUE * 4, //
    BLUE * 2, //
    BLUE * 1, //
    BLUE / 2, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN GREEN_PTN = {{
    GREEN * 1, //
    GREEN * 2, //
    GREEN * 4, //
    GREEN * 2, //
    GREEN * 1, //
    GREEN / 2, //
}};
constexpr satoh::msg::NEO_PIXEL_PATTERN RED_PTN = {{
    RED * 1, //
    RED * 2, //
    RED * 4, //
    RED * 2, //
    RED * 1, //
    RED / 2, //
}};
} // namespace

void appTaskProc(void const *argument)
{
  if (satoh::addMsgTarget(4) != osOK)
  {
    return;
  }
  satoh::msg::NEO_PIXEL_SPEED speed = {100};
  satoh::msg::LED_SIMPLE power = {0, false};
  satoh::msg::LED_SIMPLE modulation = {1, false};
  satoh::msg::SOUND_EFFECTOR effector{};
  std::unique_ptr<satoh::EffectorBase> od(new satoh::OverDrive());
  std::unique_ptr<satoh::EffectorBase> ds(new satoh::Distortion());
  std::unique_ptr<satoh::EffectorBase> tr(new satoh::Tremolo());
  std::unique_ptr<satoh::EffectorBase> ce(new satoh::Chorus());
  {
    satoh::msg::LED_ALL_EFFECT led{};
    satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_ALL_EFFECT_REQ, &led, sizeof(led));
    satoh::sendMsg(soundTaskHandle, satoh::msg::SOUND_CHANGE_EFFECTOR_REQ, &effector, sizeof(effector));
  }
  for (;;)
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
      if (param->ok == satoh::msg::BUTTON_DOWN)
      {
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &BLUE_PTN, sizeof(BLUE_PTN));
      }
      if (param->rtn == satoh::msg::BUTTON_DOWN)
      {
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &GREEN_PTN, sizeof(GREEN_PTN));
      }
      if (param->up == satoh::msg::BUTTON_DOWN)
      {
        if (10 < speed.interval)
        {
          speed.interval -= 10;
        }
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_SPEED, &speed, sizeof(speed));
      }
      if (param->down == satoh::msg::BUTTON_DOWN)
      {
        if (speed.interval <= 300)
        {
          speed.interval += 10;
        }
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_SPEED, &speed, sizeof(speed));
      }
      if (param->tap == satoh::msg::BUTTON_DOWN)
      {
        modulation.level = !modulation.level;
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_SIMPLE_REQ, &modulation, sizeof(modulation));
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &RED_PTN, sizeof(RED_PTN));
      }
      if (param->re1 == satoh::msg::BUTTON_DOWN)
      {
        power.level = !power.level;
        satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_SIMPLE_REQ, &power, sizeof(power));
        satoh::sendMsg(neoPixelTaskHandle, satoh::msg::NEO_PIXEL_SET_PATTERN, &RAINBOW_PTN, sizeof(RAINBOW_PTN));
      }
      break;
    }
    case satoh::msg::EFFECT_KEY_CHANGED_NOTIFY:
    {
      auto *param = reinterpret_cast<satoh::msg::EFFECT_KEY const *>(msg->bytes);
      satoh::EffectorBase *fx[4] = {od.get(), ds.get(), ce.get(), tr.get()};
      for (int i = 0; i < 4; ++i)
      {
        if (param->button[i] == satoh::msg::BUTTON_DOWN)
        {
          satoh::msg::LED_ALL_EFFECT led{};
          if (effector.fx[0] == fx[i])
          {
            effector.fx[0] = 0;
          }
          else
          {
            effector.fx[0] = fx[i];
            led.rgb[i] = effector.fx[0]->getColor();
          }
          satoh::sendMsg(soundTaskHandle, satoh::msg::SOUND_CHANGE_EFFECTOR_REQ, &effector, sizeof(effector));
          satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_ALL_EFFECT_REQ, &led, sizeof(led));
          break;
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
      for (uint8_t i = 0; i < 4; ++i)
      {
        if (0 < param->angleDiff[i])
        {
          if (effector.fx[0])
          {
            effector.fx[0]->incrementParam(i);
          }
        }
        if (param->angleDiff[i] < 0)
        {
          if (effector.fx[0])
          {
            effector.fx[0]->decrementParam(i);
          }
        }
      }
      break;
    }
    }
  }
}