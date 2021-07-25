/// @file      task/app_state/factory_reset.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "factory_reset.h"
#include "common/utils.h"
#include "user.h"

namespace state = satoh::state;

state::ID state::FactoryReset::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == satoh::msg::BUTTON_DOWN)
  {
    if (yes_)
    {
      m_.factoryReset();
      msg::OLED_DISP_TEXT cmd{};
      sprintf(cmd.msg1, "FACTORY RESET");
      sprintf(cmd.msg2, "DONE!!");
      sendMsg(i2cTaskHandle, msg::OLED_DISP_TEXT_REQ, &cmd, sizeof(cmd));
      osDelay(1000);
    }
    return PLAYING;
  }
  if (src->rtn == satoh::msg::BUTTON_DOWN)
  {
    return PLAYING;
  }
  if (src->up == satoh::msg::BUTTON_DOWN)
  {
    yes_ = !yes_;
    dispConfirm();
  }
  if (src->down == satoh::msg::BUTTON_DOWN)
  {
    yes_ = !yes_;
    dispConfirm();
  }
  if (src->tap == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == satoh::msg::BUTTON_DOWN)
  {
  }
  return FACTORY_RESET;
}
state::ID state::FactoryReset::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
    }
  }
  return FACTORY_RESET;
}
state::ID state::FactoryReset::run(msg::ACC_GYRO const *src) noexcept
{
  for (size_t i = 0; i < MAX_EFFECTOR_COUNT; ++i)
  {
    m_.getFx(i)->setGyroParam(src->acc);
  }
  return FACTORY_RESET;
}
state::ID state::FactoryReset::run(msg::ROTARY_ENCODER const *src) noexcept
{
  return FACTORY_RESET;
}
void state::FactoryReset::dispConfirm() const noexcept
{
  msg::OLED_DISP_CONFIRM cmd{};
  sprintf(cmd.msg1, "RESET TO FACTOORY");
  sprintf(cmd.msg2, "ARE YOU SURE?");
  cmd.yes = yes_;
  sendMsg(i2cTaskHandle, msg::OLED_DISP_CONFIRM_REQ, &cmd, sizeof(cmd));
}

void state::FactoryReset::init() noexcept
{
  yes_ = true;
  dispConfirm();
}
void state::FactoryReset::deinit() noexcept {}
