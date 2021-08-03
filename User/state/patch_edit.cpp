/// @file      state/patch_edit.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "patch_edit.h"
#include "common.h"
#include "common/utils.h"
#include "user.h"

namespace msg = satoh::msg;
namespace state = satoh::state;

state::ID state::PatchEdit::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == msg::BUTTON_DOWN)
  {
    auto *fx = m_.getEditSelectedFx();
    if (fx->getID() != fx::BYPASS)
    {
      return EFFECT_EDIT;
    }
  }
  if (src->rtn == msg::BUTTON_DOWN)
  {
    return PLAYING;
  }
  if (src->up == msg::BUTTON_DOWN)
  {
    modSelectedFxNum(false);
  }
  if (src->down == msg::BUTTON_DOWN)
  {
    modSelectedFxNum(true);
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
state::ID state::PatchEdit::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      m_.changePatch(i);
      return PLAYING;
    }
  }
  return id();
}
state::ID state::PatchEdit::run(msg::ACC_GYRO const *src) noexcept
{
  proc(m_, src);
  return id();
}
state::ID state::PatchEdit::run(msg::ROTARY_ENCODER const *src) noexcept
{
  constexpr size_t NUM_SELECT_KNOB = 0;
  constexpr size_t FX_SELECT_KNOB = 1;
  if (src->angleDiff[NUM_SELECT_KNOB] < 0)
  {
    modSelectedFxNum(false);
  }
  if (0 < src->angleDiff[NUM_SELECT_KNOB])
  {
    modSelectedFxNum(true);
  }
  if (src->angleDiff[FX_SELECT_KNOB] < 0)
  {
    modFxList(false);
  }
  if (0 < src->angleDiff[FX_SELECT_KNOB])
  {
    modFxList(true);
  }
  return id();
}
state::ID state::PatchEdit::run(msg::ERROR const *src) noexcept
{
  m_.setError(src->cause);
  return ERROR;
}
state::ID state::PatchEdit::timer() noexcept
{
  timerProc(m_);
  return id();
}
void state::PatchEdit::updateDisplay() noexcept
{
  msg::OLED_DISP_BANK cmd{};
  cmd.bank = m_.getBankNum() + 1;
  cmd.patch = m_.getPatchNum() + 1;
  cmd.editMode = true;
  cmd.selectedFx = m_.getEditSelectedFxNum() + 1;
  for (size_t i = 0; i < countof(cmd.fx); ++i)
  {
    cmd.fx[i] = m_.getFx(i);
  }
  msg::send(i2cTaskHandle, msg::OLED_DISP_BANK_REQ, &cmd, sizeof(cmd));
}
void state::PatchEdit::modSelectedFxNum(bool up) noexcept
{
  m_.updateEditSelectedFxNum(up);
  updateDisplay();
}
void state::PatchEdit::modFxList(bool next) noexcept
{
  m_.updateNextFx(next);
  updateDisplay();
  msg::SOUND_EFFECTOR sound{};
  for (size_t i = 0; i < countof(sound.fx); ++i)
  {
    sound.fx[i] = m_.getFx(i);
  }
  msg::send(soundTaskHandle, msg::SOUND_CHANGE_EFFECTOR_REQ, &sound, sizeof(sound));
}
void state::PatchEdit::init() noexcept
{
  updateDisplay();
}
void state::PatchEdit::deinit() noexcept
{
  m_.savePatch();
}
