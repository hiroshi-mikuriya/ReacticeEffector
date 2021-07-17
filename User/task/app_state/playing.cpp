/// @file      task/app_state/playing.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "playing.h"
#include "common/utils.h"
#include "message/msglib.h"
#include "user.h"

satoh::state::ID satoh::state::Playing::run(msg::MODE_KEY const *src, Property &prop) noexcept
{
  if (src->ok == satoh::msg::BUTTON_DOWN)
  {
    return PATCH_EDIT;
  }
  if (src->rtn == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->up == satoh::msg::BUTTON_DOWN)
  {
    prop.bankNum = (prop.bankNum + MAX_BANK + 1) % MAX_BANK;
    modBank(prop);
  }
  if (src->down == satoh::msg::BUTTON_DOWN)
  {
    prop.bankNum = (prop.bankNum + MAX_BANK - 1) % MAX_BANK;
    modBank(prop);
  }
  if (src->tap == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == satoh::msg::BUTTON_DOWN)
  {
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::EFFECT_KEY const *src, Property &prop) noexcept
{
  for (uint8_t i = 0; i < satoh::countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      prop.patchNum = i;
      modBank(prop);
      break;
    }
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::ACC_GYRO const *src, Property &prop) noexcept
{
  auto &pch = prop.getCurrectPatch();
  for (auto *fx : pch.fx)
  {
    if (fx)
    {
      fx->setGyroParam(src->acc);
    }
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::ROTARY_ENCODER const *src, Property &prop) noexcept
{
  return PLAYING;
}
void satoh::state::Playing::modBank(Property &prop) noexcept
{
  msg::OLED_DISP_BANK disp{};
  disp.bank = prop.bankNum + 1;
  disp.patch = prop.patchNum + 1;
  for (size_t i = 0; i < satoh::countof(disp.fx); ++i)
  {
    disp.fx[i] = prop.getCurrectPatch().fx[i];
  }
  sendMsg(i2cTaskHandle, msg::OLED_DISP_BANK_REQ, &disp, sizeof(disp));
  msg::SOUND_EFFECTOR sound{};
  for (size_t i = 0; i < satoh::countof(sound.fx); ++i)
  {
    sound.fx[i] = disp.fx[i];
  }
  sendMsg(soundTaskHandle, msg::SOUND_CHANGE_EFFECTOR_REQ, &sound, sizeof(sound));
}
void satoh::state::Playing::init(Property &prop) noexcept
{
  modBank(prop);
}
void satoh::state::Playing::deinit(Property &prop) noexcept
{
  // TODO
}
