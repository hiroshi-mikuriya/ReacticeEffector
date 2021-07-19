/// @file      task/app_state/patch_edit.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "patch_edit.h"
#include "common/utils.h"
#include "user.h"

satoh::state::ID satoh::state::PatchEdit::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == satoh::msg::BUTTON_DOWN)
  {
    auto *fx = m_.getCurrectPatch().fx[m_.editSelectedFxNum];
    if (fx->getID() != fx::BYPASS)
    {
      return EFFECT_EDIT;
    }
  }
  if (src->rtn == satoh::msg::BUTTON_DOWN)
  {
    m_.getCurrectPatch().dump();
    return PLAYING;
  }
  if (src->up == satoh::msg::BUTTON_DOWN)
  {
    modSelectedFxNum(false);
  }
  if (src->down == satoh::msg::BUTTON_DOWN)
  {
    modSelectedFxNum(true);
  }
  if (src->tap == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == satoh::msg::BUTTON_DOWN)
  {
  }
  return PATCH_EDIT;
}
satoh::state::ID satoh::state::PatchEdit::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < satoh::countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      m_.getCurrectPatch().dump();
      m_.patchNum = i;
      return PLAYING;
    }
  }
  return PATCH_EDIT;
}
satoh::state::ID satoh::state::PatchEdit::run(msg::ACC_GYRO const *src) noexcept
{
  for (auto *fx : m_.getCurrectPatch().fx)
  {
    fx->setGyroParam(src->acc);
  }
  return PATCH_EDIT;
}
satoh::state::ID satoh::state::PatchEdit::run(msg::ROTARY_ENCODER const *src) noexcept
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
  return PATCH_EDIT;
}
void satoh::state::PatchEdit::updateDisplay() noexcept
{
  msg::OLED_DISP_BANK cmd{};
  cmd.bank = m_.bankNum + 1;
  cmd.patch = m_.patchNum + 1;
  cmd.editMode = true;
  cmd.selectedFx = m_.editSelectedFxNum + 1;
  for (size_t i = 0; i < satoh::countof(cmd.fx); ++i)
  {
    cmd.fx[i] = m_.getCurrectPatch().fx[i];
  }
  sendMsg(i2cTaskHandle, msg::OLED_DISP_BANK_REQ, &cmd, sizeof(cmd));
}
void satoh::state::PatchEdit::modSelectedFxNum(bool up) noexcept
{
  if (up)
  {
    m_.editSelectedFxNum = (m_.editSelectedFxNum + 1) % MAX_EFFECTOR_COUNT;
  }
  else
  {
    m_.editSelectedFxNum = (m_.editSelectedFxNum + MAX_EFFECTOR_COUNT - 1) % MAX_EFFECTOR_COUNT;
  }
  updateDisplay();
}
void satoh::state::PatchEdit::modFxList(bool next) noexcept
{
  auto &pch = m_.getCurrectPatch();
  auto *cur = pch.fx[m_.editSelectedFxNum];
  auto *fx = m_.effectors->get(cur, next);
  pch.fx[m_.editSelectedFxNum] = fx;
  updateDisplay();
  msg::SOUND_EFFECTOR sound{};
  for (size_t i = 0; i < satoh::countof(sound.fx); ++i)
  {
    sound.fx[i] = pch.fx[i];
  }
  sendMsg(soundTaskHandle, msg::SOUND_CHANGE_EFFECTOR_REQ, &sound, sizeof(sound));
}
void satoh::state::PatchEdit::init() noexcept
{
  m_.editSelectedFxNum = 0;
  updateDisplay();
}
void satoh::state::PatchEdit::deinit() noexcept
{
  // TODO
}
