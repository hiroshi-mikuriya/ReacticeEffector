/// @file      state/common.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "common.h"
#include "message/type.h"
#include "user.h"

namespace msg = satoh::msg;
namespace state = satoh::state;

namespace
{
constexpr uint8_t V = 0x20;
constexpr satoh::RGB RED = {V, 0, 0};
constexpr satoh::RGB YELLOW = {V, V, 0};
constexpr satoh::RGB GREEN = {0, V, 0};
constexpr satoh::RGB AQUA = {0, V, V};
constexpr satoh::RGB BLUE = {0, 0, V};
constexpr satoh::RGB PINK = {V, 0, V};
constexpr msg::NEO_PIXEL_PATTERN RAINBOW_PTN = {{
    RED,    //
    YELLOW, //
    GREEN,  //
    AQUA,   //
    BLUE,   //
    PINK,   //
}};
constexpr msg::NEO_PIXEL_PATTERN BLUE_PTN = {{
    BLUE * 1, //
    BLUE * 2, //
    BLUE * 4, //
    BLUE * 2, //
    BLUE * 1, //
    BLUE / 2, //
}};
constexpr msg::NEO_PIXEL_PATTERN GREEN_PTN = {{
    GREEN * 1, //
    GREEN * 2, //
    GREEN * 4, //
    GREEN * 2, //
    GREEN * 1, //
    GREEN / 2, //
}};
constexpr msg::NEO_PIXEL_PATTERN RED_PTN = {{
    RED * 1, //
    RED * 2, //
    RED * 4, //
    RED * 2, //
    RED * 1, //
    RED / 2, //
}};
/// @brief NeoPixel点灯パターンを変更する
void changeNeoPixelPattern() noexcept
{
  static int n = 0;
  switch (n)
  {
  case 0:
    msg::send(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &RAINBOW_PTN, sizeof(RAINBOW_PTN));
    break;
  case 1:
    msg::send(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &RED_PTN, sizeof(RED_PTN));
    break;
  case 2:
    msg::send(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &GREEN_PTN, sizeof(GREEN_PTN));
    break;
  case 3:
    msg::send(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &BLUE_PTN, sizeof(BLUE_PTN));
    break;
  case 4:
  {
    msg::NEO_PIXEL_PATTERN ptn{};
    msg::send(neoPixelTaskHandle, msg::NEO_PIXEL_SET_PATTERN, &ptn, sizeof(ptn));
    break;
  }
  }
  n = (n + 1) % 5;
}
} // namespace

void state::proc(Property &prop, msg::ACC_GYRO const *src) noexcept
{
  for (size_t i = 0; i < MAX_EFFECTOR_COUNT; ++i)
  {
    prop.getFx(i)->setGyroParam(src->acc);
  }
}

void state::tapProc(Property &prop) noexcept
{
  for (size_t i = 0; i < MAX_EFFECTOR_COUNT; ++i)
  {
    prop.getFx(i)->tap();
  }
  // TODO 暫定
  uint32_t tick = osKernelSysTick();
  uint32_t d = (tick - prop.lastTapTick) / 4;
  if (0 < d && d < 1000)
  {
    msg::NEO_PIXEL_SPEED speed = {d};
    msg::send(neoPixelTaskHandle, msg::NEO_PIXEL_SET_SPEED, &speed, sizeof(speed));
  }
  prop.lastTapTick = tick;
}

void state::re1Proc(Property &prop) noexcept
{
  static_cast<void>(prop); // unused
  changeNeoPixelPattern();
}

void state::timerProc(Property &prop) noexcept
{
  // TODO
}
