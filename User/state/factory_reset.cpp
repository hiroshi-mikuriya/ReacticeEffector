/// @file      state/factory_reset.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "factory_reset.h"
#include "common.h"
#include "common/utils.h"
#include "user.h"

namespace msg = satoh::msg;

namespace state = satoh::state;

namespace
{
/// @brief ファクトリーリセットを実行する
void resetParams(state::Property &prop)
{
  prop.factoryReset();
  msg::OLED_DISP_TEXT cmd{};
  sprintf(cmd.msg1, "FACTORY RESET");
  sprintf(cmd.msg2, "DONE!!");
  msg::send(i2cTaskHandle, msg::OLED_DISP_TEXT_REQ, &cmd, sizeof(cmd));
  osDelay(1000);
}
} // namespace

state::ID state::FactoryReset::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == msg::BUTTON_DOWN)
  {
    if (yes_)
    {
      resetParams(m_);
    }
    return PLAYING;
  }
  if (src->rtn == msg::BUTTON_DOWN)
  {
    return PLAYING;
  }
  if (src->up == msg::BUTTON_DOWN)
  {
    yes_ = !yes_;
    dispConfirm();
  }
  if (src->down == msg::BUTTON_DOWN)
  {
    yes_ = !yes_;
    dispConfirm();
  }
  if (src->tap == msg::BUTTON_DOWN)
  {
    tapProc(m_);
  }
  if (src->re1 == msg::BUTTON_DOWN)
  {
    re1Proc(m_);
  }
  return id();
}
state::ID state::FactoryReset::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
    }
  }
  return id();
}
state::ID state::FactoryReset::run(msg::ACC_GYRO const *src) noexcept
{
  proc(m_, src);
  return id();
}
state::ID state::FactoryReset::run(msg::ROTARY_ENCODER const *src) noexcept
{
  for (auto angle : src->angleDiff)
  {
    if (0 < angle)
    {
      yes_ = false;
      dispConfirm();
    }
    if (angle < 0)
    {
      yes_ = true;
      dispConfirm();
    }
  }
  return id();
}
state::ID state::FactoryReset::run(msg::ERROR const *src) noexcept
{
  m_.setError(src->cause);
  return ERROR;
}
state::ID state::FactoryReset::timer() noexcept
{
  timerProc(m_);
  return id();
}
void state::FactoryReset::dispConfirm() const noexcept
{
  msg::OLED_DISP_CONFIRM cmd{};
  strcpy(cmd.msg1, "RESET TO FACTOORY");
  strcpy(cmd.msg2, "ARE YOU SURE?");
  cmd.yes = yes_;
  msg::send(i2cTaskHandle, msg::OLED_DISP_CONFIRM_REQ, &cmd, sizeof(cmd));
}

void state::FactoryReset::init() noexcept
{
  yes_ = false;
  dispConfirm();
}
void state::FactoryReset::deinit() noexcept {}
