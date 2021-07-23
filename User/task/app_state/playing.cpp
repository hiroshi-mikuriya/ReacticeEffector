/// @file      task/app_state/playing.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "playing.h"
#include "common/utils.h"
#include "message/msglib.h"
#include "user.h"

namespace
{
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
    m_.changeBank(true);
    modBank();
  }
  if (src->down == satoh::msg::BUTTON_DOWN)
  {
    m_.changeBank(false);
    modBank();
  }
  if (src->tap == satoh::msg::BUTTON_DOWN)
  {
    for (size_t i = 0; i < MAX_EFFECTOR_COUNT; ++i)
    {
      m_.getFx(i)->tap();
    }
    // TODO 暫定
    uint32_t tick = osKernelSysTick();
    uint32_t d = (tick - m_.lastTapTick) / 4;
    if (0 < d && d < 1000)
    {
      msg::NEO_PIXEL_SPEED speed = {d};
      sendMsg(neoPixelTaskHandle, msg::NEO_PIXEL_SET_SPEED, &speed, sizeof(speed));
    }
    m_.lastTapTick = tick;
  }
  if (src->re1 == satoh::msg::BUTTON_DOWN)
  {
    // TODO 暫定
    static int n = 0;
    switch (n)
    {
    case 0:
      sendMsg(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &RAINBOW_PTN, sizeof(RAINBOW_PTN));
      break;
    case 1:
      sendMsg(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &RED_PTN, sizeof(RED_PTN));
      break;
    case 2:
      sendMsg(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &GREEN_PTN, sizeof(GREEN_PTN));
      break;
    case 3:
      sendMsg(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &BLUE_PTN, sizeof(BLUE_PTN));
      break;
    case 4:
    {
      msg::NEO_PIXEL_PATTERN ptn{};
      sendMsg(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &ptn, sizeof(ptn));
      break;
    }
    }
    n = (n + 1) % 5;
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < satoh::countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      m_.changePatch(i);
      modBank();
      break;
    }
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::ACC_GYRO const *src) noexcept
{
  for (size_t i = 0; i < MAX_EFFECTOR_COUNT; ++i)
  {
    m_.getFx(i)->setGyroParam(src->acc);
  }
  return PLAYING;
}
satoh::state::ID satoh::state::Playing::run(msg::ROTARY_ENCODER const *src) noexcept
{
  return PLAYING;
}
void satoh::state::Playing::modBank() noexcept
{
  msg::OLED_DISP_BANK disp{};
  disp.bank = m_.getBankNum() + 1;
  disp.patch = m_.getPatchNum() + 1;
  for (size_t i = 0; i < satoh::countof(disp.fx); ++i)
  {
    disp.fx[i] = m_.getFx(i);
  }
  sendMsg(i2cTaskHandle, msg::OLED_DISP_BANK_REQ, &disp, sizeof(disp));
  msg::SOUND_EFFECTOR sound{};
  for (size_t i = 0; i < satoh::countof(sound.fx); ++i)
  {
    sound.fx[i] = disp.fx[i];
  }
  sendMsg(soundTaskHandle, msg::SOUND_CHANGE_EFFECTOR_REQ, &sound, sizeof(sound));
  msg::LED_ALL_EFFECT led{};
  constexpr satoh::RGB colorList[] = {
      satoh::RGB{0x20, 0x00, 0x00}, //
      satoh::RGB{0x00, 0x20, 0x00}, //
      satoh::RGB{0x00, 0x00, 0x20}, //
      satoh::RGB{0x20, 0x20, 0x00}, //
  };
  led.rgb[m_.getPatchNum()] = colorList[m_.getPatchNum()];
  sendMsg(i2cTaskHandle, msg::LED_ALL_EFFECT_REQ, &led, sizeof(led));
}
void satoh::state::Playing::init() noexcept
{
  modBank();
  m_.initEditSelectedFxNum();
}
void satoh::state::Playing::deinit() noexcept
{
  // TODO
}
