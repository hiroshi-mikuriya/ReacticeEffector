/// @file      state/effect_edit.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "effect_edit.h"
#include "common.h"
#include "common/utils.h"

namespace msg = satoh::msg;
namespace state = satoh::state;

state::ID state::EffectEdit::run(msg::MODE_KEY const *src) noexcept
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
    tapProc(m_);
  }
  if (src->re1 == msg::BUTTON_DOWN)
  {
    re1Proc(m_);
  }
  return id();
}
state::ID state::EffectEdit::run(msg::EFFECT_KEY const *src) noexcept
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
state::ID state::EffectEdit::run(msg::ACC_GYRO const *src) noexcept
{
  proc(m_, src);
  return id();
}
state::ID state::EffectEdit::run(msg::ROTARY_ENCODER const *src) noexcept
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
  if (0 < paramKnob && fx->getExp(selectedParamNum_) == EXP_NONE)
  {
    fx->incrementParam(selectedParamNum_);
    showFx();
  }
  if (paramKnob < 0 && fx->getExp(selectedParamNum_) == EXP_NONE)
  {
    fx->decrementParam(selectedParamNum_);
    showFx();
  }
  int8_t gyroKnob = src->angleDiff[2];
  if (0 < gyroKnob)
  {
    fx->setExp(selectedParamNum_, EXP_GYRO);
    showFx();
  }
  if (gyroKnob < 0)
  {
    fx->setExp(selectedParamNum_, EXP_NONE);
    showFx();
  }
  return id();
}
state::ID state::EffectEdit::run(msg::ERROR const *src) noexcept
{
  m_.setError(src->cause);
  return ERROR;
}
state::ID state::EffectEdit::timer() noexcept
{
  timerProc(m_);
  return id();
}
void state::EffectEdit::modSelectedParam(bool up)
{
  auto *fx = m_.getEditSelectedFx();
  int8_t d = up ? 1 : -1;
  selectedParamNum_ = (selectedParamNum_ + fx->getParamCount() + d) % fx->getParamCount();
  showFx();
}
void state::EffectEdit::showFx() const noexcept
{
  auto *fx = m_.getEditSelectedFx();
  msg::OLED_DISP_EFFECTOR cmd{};
  cmd.fx = fx;
  cmd.patch = m_.getEditSelectedFxNum() + 1;
  cmd.selectedParam = selectedParamNum_;
  msg::send(i2cTaskHandle, msg::OLED_DISP_EFFECTOR_REQ, cmd);
}
void state::EffectEdit::init() noexcept
{
  selectedParamNum_ = 0;
  showFx();
}
void state::EffectEdit::deinit() noexcept
{
  m_.savePatch();
}
