/// @file      task/app_state/patch_edit.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "patch_edit.h"
#include "common/utils.h"
#include "user.h"

satoh::state::ID satoh::state::PatchEdit::run(msg::MODE_KEY const *src, Property &prop) noexcept
{
  if (src->ok == satoh::msg::BUTTON_DOWN)
  {
    auto *fx = prop.getCurrectPatch().fx[prop.editSelectedFxNum];
    if (fx->getID() != fx::BYPASS)
    {
      return EFFECT_EDIT;
    }
  }
  if (src->rtn == satoh::msg::BUTTON_DOWN)
  {
    return PLAYING;
  }
  if (src->up == satoh::msg::BUTTON_DOWN)
  {
    modSelectedFx(false, prop);
  }
  if (src->down == satoh::msg::BUTTON_DOWN)
  {
    modSelectedFx(true, prop);
  }
  if (src->tap == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == satoh::msg::BUTTON_DOWN)
  {
  }
  return PATCH_EDIT;
}
satoh::state::ID satoh::state::PatchEdit::run(msg::EFFECT_KEY const *src, Property &prop) noexcept
{
  for (uint8_t i = 0; i < satoh::countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      prop.patchNum = i;
      return PLAYING;
    }
  }
  return PATCH_EDIT;
}
satoh::state::ID satoh::state::PatchEdit::run(msg::ACC_GYRO const *src, Property &prop) noexcept
{
  auto &pch = prop.getCurrectPatch();
  for (auto *fx : pch.fx)
  {
    if (fx)
    {
      fx->setGyroParam(src->acc);
    }
  }
  return PATCH_EDIT;
}
satoh::state::ID satoh::state::PatchEdit::run(msg::ROTARY_ENCODER const *src, Property &prop) noexcept
{
  constexpr size_t NUM_SELECT_KNOB = 0;
  constexpr size_t FX_SELECT_KNOB = 1;
  if (src->angleDiff[NUM_SELECT_KNOB] < 0)
  {
    modSelectedFx(false, prop);
  }
  if (0 < src->angleDiff[NUM_SELECT_KNOB])
  {
    modSelectedFx(true, prop);
  }
  if (src->angleDiff[FX_SELECT_KNOB] < 0)
  {
  }
  if (0 < src->angleDiff[FX_SELECT_KNOB])
  {
  }
  return PATCH_EDIT;
}
void satoh::state::PatchEdit::modBank(Property &prop) noexcept
{
  msg::OLED_DISP_BANK cmd{};
  cmd.bank = prop.bankNum + 1;
  cmd.patch = prop.patchNum + 1;
  cmd.editMode = true;
  cmd.selectedFx = prop.editSelectedFxNum + 1;
  for (size_t i = 0; i < satoh::countof(cmd.fx); ++i)
  {
    cmd.fx[i] = prop.getCurrectPatch().fx[i];
  }
  sendMsg(i2cTaskHandle, msg::OLED_DISP_BANK_REQ, &cmd, sizeof(cmd));
}
void satoh::state::PatchEdit::modSelectedFx(bool up, Property &prop) noexcept
{
  if (up)
  {
    prop.editSelectedFxNum = (prop.editSelectedFxNum + 1) % MAX_EFFECTOR_COUNT;
  }
  else
  {
    prop.editSelectedFxNum = (prop.editSelectedFxNum + MAX_EFFECTOR_COUNT - 1) % MAX_EFFECTOR_COUNT;
  }
  modBank(prop);
}
void satoh::state::PatchEdit::init(Property &prop) noexcept
{
  prop.editSelectedFxNum = 0;
  modBank(prop);
}
void satoh::state::PatchEdit::deinit(Property &prop) noexcept
{
  // TODO
}
