/// @file      state/playing.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "playing.h"
#include "common.h"
#include "common/dma_mem.h"
#include "common/utils.h"
#include "user.h"

namespace msg = satoh::msg;
namespace state = satoh::state;

state::ID state::Playing::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == msg::BUTTON_DOWN)
  {
    return PATCH_EDIT;
  }
  if (src->rtn == msg::BUTTON_DOWN)
  {
  }
  if (src->up == msg::BUTTON_DOWN)
  {
    m_.changeBank(true);
    modBank();
  }
  if (src->down == msg::BUTTON_DOWN)
  {
    m_.changeBank(false);
    modBank();
  }
  if (src->tap == msg::BUTTON_DOWN)
  {
    tapProc(m_);
  }
  if (src->re1 == msg::BUTTON_DOWN)
  {
    {
      // 隠し機能
      char msg[64] = {0};
      int n = sprintf(msg, "[FREE SIZE] rtos: %d dtcm: %d\r\n", xPortGetFreeHeapSize(), satoh::getFreeDmaMemSize());
      msg::send(usbTxTaskHandle, msg::USB_TX_REQ, msg, n);
    }
    re1Proc(m_);
  }
  return id();
}
state::ID state::Playing::run(msg::EFFECT_KEY const *src) noexcept
{
  if (src->button[2] && src->button[3])
  {
    return FACTORY_RESET;
  }
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      m_.changePatch(i);
      modBank();
      break;
    }
  }
  return id();
}
state::ID state::Playing::run(msg::ACC_GYRO const *src) noexcept
{
  proc(m_, src);
  return id();
}
state::ID state::Playing::run(msg::ROTARY_ENCODER const *src) noexcept
{
  return id();
}
state::ID state::Playing::run(msg::ERROR const *src) noexcept
{
  m_.setError(src->cause);
  return ERROR;
}
void state::Playing::modBank() noexcept
{
  msg::OLED_DISP_BANK disp{};
  disp.bank = m_.getBankNum() + 1;
  disp.patch = m_.getPatchNum() + 1;
  for (size_t i = 0; i < countof(disp.fx); ++i)
  {
    disp.fx[i] = m_.getFx(i);
  }
  msg::send(i2cTaskHandle, msg::OLED_DISP_BANK_REQ, &disp, sizeof(disp));
  msg::SOUND_EFFECTOR sound{};
  for (size_t i = 0; i < countof(sound.fx); ++i)
  {
    sound.fx[i] = disp.fx[i];
  }
  msg::send(soundTaskHandle, msg::SOUND_CHANGE_EFFECTOR_REQ, &sound, sizeof(sound));
  msg::LED_ALL_EFFECT led{};
  led.rgb[m_.getPatchNum()] = m_.getCurrentColor();
  msg::send(i2cTaskHandle, msg::LED_ALL_EFFECT_REQ, &led, sizeof(led));
}
void state::Playing::init() noexcept
{
  modBank();
  m_.initEditSelectedFxNum();
  msg::LED_SIMPLE led{0, true};
  msg::send(i2cTaskHandle, msg::LED_SIMPLE_REQ, &led, sizeof(led));
}
void state::Playing::deinit() noexcept
{
  // TODO
}
