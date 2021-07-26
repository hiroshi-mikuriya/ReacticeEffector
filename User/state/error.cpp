/// @file      state/error.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "error.h"
#include "common/utils.h"
#include "user.h"

namespace msg = satoh::msg;

satoh::state::ID satoh::state::Error::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == msg::BUTTON_DOWN)
  {
  }
  if (src->rtn == msg::BUTTON_DOWN)
  {
  }
  if (src->up == msg::BUTTON_DOWN)
  {
  }
  if (src->down == msg::BUTTON_DOWN)
  {
  }
  if (src->tap == msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == msg::BUTTON_DOWN)
  {
  }
  return ERROR;
}
satoh::state::ID satoh::state::Error::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
  }
  return ERROR;
}
satoh::state::ID satoh::state::Error::run(msg::ACC_GYRO const *src) noexcept
{
  return ERROR;
}
satoh::state::ID satoh::state::Error::run(msg::ROTARY_ENCODER const *src) noexcept
{
  return ERROR;
}
void satoh::state::Error::init() noexcept
{
  // TOOD
}
void satoh::state::Error::deinit() noexcept
{
  // TODO
}
