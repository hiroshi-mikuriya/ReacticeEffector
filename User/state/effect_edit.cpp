/// @file      state/effect_edit.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "effect_edit.h"
#include "common/utils.h"
#include "user.h"

namespace msg = satoh::msg;

satoh::state::ID satoh::state::EffectEdit::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == msg::BUTTON_DOWN)
  {
  }
  if (src->rtn == msg::BUTTON_DOWN)
  {
    return PATCH_EDIT;
  }
  if (src->up == msg::BUTTON_DOWN)
  {
    modSelectedParam(false);
  }
  if (src->down == msg::BUTTON_DOWN)
  {
    modSelectedParam(true);
  }
  if (src->tap == msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == msg::BUTTON_DOWN)
  {
  }
  return EFFECT_EDIT;
}
satoh::state::ID satoh::state::EffectEdit::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
      m_.changePatch(i);
      return PLAYING;
    }
  }
  return EFFECT_EDIT;
}
satoh::state::ID satoh::state::EffectEdit::run(msg::ACC_GYRO const *src) noexcept
{
  for (size_t i = 0; i < MAX_EFFECTOR_COUNT; ++i)
  {
    m_.getFx(i)->setGyroParam(src->acc);
  }
  return EFFECT_EDIT;
}
satoh::state::ID satoh::state::EffectEdit::run(msg::ROTARY_ENCODER const *src) noexcept
{
  auto *fx = m_.getEditSelectedFx();
  int8_t selectKnob = src->angleDiff[0];
  if (0 < selectKnob)
  {
    modSelectedParam(true);
  }
  if (selectKnob < 0)
  {
    modSelectedParam(false);
  }
  int8_t paramKnob = src->angleDiff[1];
  if (0 < paramKnob && !fx->isGyroEnabled(selectedParamNum_))
  {
    fx->incrementParam(selectedParamNum_);
    msg::send(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  if (paramKnob < 0 && !fx->isGyroEnabled(selectedParamNum_))
  {
    fx->decrementParam(selectedParamNum_);
    msg::send(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  int8_t gyroKnob = src->angleDiff[2];
  if (0 < gyroKnob)
  {
    fx->setGyroEnable(selectedParamNum_, true);
    msg::send(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  if (gyroKnob < 0)
  {
    fx->setGyroEnable(selectedParamNum_, false);
    msg::send(i2cTaskHandle, msg::OLED_UPDATE_PARAM_REQ);
  }
  return EFFECT_EDIT;
}
void satoh::state::EffectEdit::modSelectedParam(bool up)
{
  auto *fx = m_.getEditSelectedFx();
  int8_t d = up ? 1 : -1;
  selectedParamNum_ = (selectedParamNum_ + fx->getParamCount() + d) % fx->getParamCount();
  msg::OLED_SELECT_PARAM cmd{selectedParamNum_};
  msg::send(i2cTaskHandle, msg::OLED_SELECT_PARAM_REQ, &cmd, sizeof(cmd));
}
void satoh::state::EffectEdit::init() noexcept
{
  selectedParamNum_ = 0;
  auto *fx = m_.getEditSelectedFx();
  msg::OLED_DISP_EFFECTOR cmd{};
  cmd.fx = fx;
  cmd.patch = m_.getEditSelectedFxNum() + 1;
  msg::send(i2cTaskHandle, msg::OLED_DISP_EFFECTOR_REQ, &cmd, sizeof(cmd));
}
void satoh::state::EffectEdit::deinit() noexcept
{
  m_.savePatch();
}
