/// @file      task/app_state/playing.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "playing.h"
#include "common/utils.h"
#include "message/msglib.h"
#include "user.h"

satoh::state::ID satoh::state::Playing::run(msg::MODE_KEY const *src) noexcept
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
    m_.getCurrectPatch().dump();
    m_.bankNum = (m_.bankNum + MAX_BANK + 1) % MAX_BANK;
    modBank();
  }
  if (src->down == satoh::msg::BUTTON_DOWN)
  {
    m_.getCurrectPatch().dump();
    m_.bankNum = (m_.bankNum + MAX_BANK - 1) % MAX_BANK;
    modBank();
  }
  if (src->tap == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == satoh::msg::BUTTON_DOWN)
  {
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < satoh::countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      m_.getCurrectPatch().dump();
      m_.patchNum = i;
      modBank();
      break;
    }
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::ACC_GYRO const *src) noexcept
{
  for (auto *fx : m_.getCurrectPatch().fx)
  {
    fx->setGyroParam(src->acc);
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::ROTARY_ENCODER const *src) noexcept
{
  return PLAYING;
}
void satoh::state::Playing::modBank() noexcept
{
  m_.getCurrectPatch().load();
  msg::OLED_DISP_BANK disp{};
  disp.bank = m_.bankNum + 1;
  disp.patch = m_.patchNum + 1;
  for (size_t i = 0; i < satoh::countof(disp.fx); ++i)
  {
    disp.fx[i] = m_.getCurrectPatch().fx[i];
  }
  sendMsg(i2cTaskHandle, msg::OLED_DISP_BANK_REQ, &disp, sizeof(disp));
  msg::SOUND_EFFECTOR sound{};
  for (size_t i = 0; i < satoh::countof(sound.fx); ++i)
  {
    sound.fx[i] = disp.fx[i];
  }
  sendMsg(soundTaskHandle, msg::SOUND_CHANGE_EFFECTOR_REQ, &sound, sizeof(sound));
  msg::LED_ALL_EFFECT led{};
  switch (m_.patchNum)
  {
  case 0:
    led.rgb[0] = satoh::RGB{0x20, 0x00, 0x00};
    break;
  case 1:
    led.rgb[1] = satoh::RGB{0x00, 0x20, 0x00};
    break;
  case 2:
    led.rgb[2] = satoh::RGB{0x00, 0x00, 0x20};
    break;
  case 3:
    led.rgb[3] = satoh::RGB{0x20, 0x20, 0x00};
    break;
  }
  sendMsg(i2cTaskHandle, msg::LED_ALL_EFFECT_REQ, &led, sizeof(led));
}
void satoh::state::Playing::init() noexcept
{
  modBank();
}
void satoh::state::Playing::deinit() noexcept
{
  // TODO
}
