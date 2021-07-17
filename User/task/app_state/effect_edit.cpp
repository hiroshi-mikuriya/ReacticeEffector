/// @file      task/app_state/effect_edit.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "effect_edit.h"
#include "common/utils.h"
#include "user.h"

satoh::state::ID satoh::state::EffectEdit::run(msg::MODE_KEY const *src, Property &prop) noexcept
{
  if (src->ok == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->rtn == satoh::msg::BUTTON_DOWN)
  {
    return PATCH_EDIT;
  }
  if (src->up == satoh::msg::BUTTON_DOWN)
  {
    modSelectedParam(false, prop);
  }
  if (src->down == satoh::msg::BUTTON_DOWN)
  {
    modSelectedParam(true, prop);
  }
  if (src->tap == satoh::msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == satoh::msg::BUTTON_DOWN)
  {
  }
  return EFFECT_EDIT;
}
satoh::state::ID satoh::state::EffectEdit::run(msg::EFFECT_KEY const *src, Property &prop) noexcept
{
  for (uint8_t i = 0; i < satoh::countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      prop.patchNum = i;
      return PLAYING;
    }
  }
  return EFFECT_EDIT;
}
satoh::state::ID satoh::state::EffectEdit::run(msg::ACC_GYRO const *src, Property &prop) noexcept
{
  auto &pch = prop.getCurrectPatch();
  for (auto *fx : pch.fx)
  {
    if (fx)
    {
      fx->setGyroParam(src->acc);
    }
  }
  return EFFECT_EDIT;
}
satoh::state::ID satoh::state::EffectEdit::run(msg::ROTARY_ENCODER const *src, Property &prop) noexcept
{
  auto *fx = prop.getCurrectPatch().fx[prop.editSelectedFxNum];
  int8_t selectKnob = src->angleDiff[0];
  if (0 < selectKnob)
  {
    modSelectedParam(true, prop);
  }
  if (selectKnob < 0)
  {
    modSelectedParam(false, prop);
  }
  int8_t paramKnob = src->angleDiff[1];
  if (0 < paramKnob && !fx->isGyroEnabled(selectedParamNum_))
  {
    fx->incrementParam(selectedParamNum_);
    sendMsg(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  if (paramKnob < 0 && !fx->isGyroEnabled(selectedParamNum_))
  {
    fx->decrementParam(selectedParamNum_);
    sendMsg(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  int8_t gyroKnob = src->angleDiff[2];
  if (0 < gyroKnob)
  {
    fx->setGyroEnable(selectedParamNum_, true);
    sendMsg(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  if (gyroKnob < 0)
  {
    fx->setGyroEnable(selectedParamNum_, false);
    sendMsg(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  return EFFECT_EDIT;
}
void satoh::state::EffectEdit::modSelectedParam(bool up, Property &prop)
{
  auto *fx = prop.getCurrectPatch().fx[prop.editSelectedFxNum];
  int8_t d = up ? 1 : -1;
  selectedParamNum_ = (selectedParamNum_ + fx->getParamCount() + d) % fx->getParamCount();
  msg::OLED_SELECT_PARAM cmd{selectedParamNum_};
  sendMsg(i2cTaskHandle, msg::OLED_SELECT_PARAM_REQ, &cmd, sizeof(cmd));
}
void satoh::state::EffectEdit::init(Property &prop) noexcept
{
  selectedParamNum_ = 0;
  auto *fx = prop.getCurrectPatch().fx[prop.editSelectedFxNum];
  msg::OLED_DISP_EFFECTOR cmd{};
  cmd.fx = fx;
  cmd.patch = prop.editSelectedFxNum + 1;
  sendMsg(i2cTaskHandle, msg::OLED_DISP_EFFECTOR_REQ, &cmd, sizeof(cmd));
}
void satoh::state::EffectEdit::deinit(Property &prop) noexcept
{
  // TODO
}
