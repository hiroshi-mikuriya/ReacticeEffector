/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/app_task.h"
#include "message/msglib.h"
#include "task/i2c_task.h"
#include "task/usb_task.h"
#include <cstdlib> // rand

void appTaskProc(void const *argument)
{
  satoh::addMsgTarget(4);
  constexpr int MAX_LEVEL = 8;
  satoh::msg::LED_LEVEL level{};
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
    case satoh::msg::KEY_NOTIFY:
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
      // レベルメーター
      level.left = (level.left + MAX_LEVEL + param->angleDiff[2]) % MAX_LEVEL;
      level.right = (level.right + MAX_LEVEL + param->angleDiff[3]) % MAX_LEVEL;
      satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_LEVEL_UPDATE_REQ, &level, sizeof(level));
      // POWER LED, MODULATION LED
      for (uint8_t i = 0; i < 2; ++i)
      {
        if (param->angleDiff[i] != 0)
        {
          satoh::msg::LED_SIMPLE src{};
          src.led = i;
          src.level = 0 < param->angleDiff[i];
          satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_SIMPLE_REQ, &src, sizeof(src));
        }
      }
      break;
    }
    }
  }
}