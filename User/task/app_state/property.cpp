/// @file      task/app_state/property.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "property.h"
#include "factory_reset.h"

namespace state = satoh::state;
namespace fx = satoh::fx;

// state::Effectors

state::Effectors::Effectors(uint8_t n) : count_(0)
{
  // TODO n は何か使う可能性がある気がするのでとっておく
  if (bypass_.ok())
  {
    list_[count_++] = &bypass_;
  }
  if (booster_.ok())
  {
    list_[count_++] = &booster_;
  }
  if (overDrive_.ok())
  {
    list_[count_++] = &overDrive_;
  }
  if (distortion_.ok())
  {
    list_[count_++] = &distortion_;
  }
  if (chorus_.ok())
  {
    list_[count_++] = &chorus_;
  }
  if (phaser_.ok())
  {
    list_[count_++] = &phaser_;
  }
  if (tremolo_.ok())
  {
    list_[count_++] = &tremolo_;
  }
  if (delay_.ok())
  {
    list_[count_++] = &delay_;
  }
  if (oscillator_.ok())
  {
    list_[count_++] = &oscillator_;
  }
  if (bqFilter_.ok())
  {
    list_[count_++] = &bqFilter_;
  }
}

fx::EffectorBase *state::Effectors::getFx(size_t i) noexcept
{
  return list_[i % count()];
}

fx::EffectorBase const *state::Effectors::getFx(size_t i) const noexcept
{
  return const_cast<Effectors *>(this)->getFx(i);
}

fx::EffectorBase *state::Effectors::getFxById(fx::ID id) noexcept
{
  for (size_t i = 0; i < count(); ++i)
  {
    auto *a = getFx(i);
    if (id == a->getID())
    {
      return a;
    }
  }
  return getFx(0);
}

fx::EffectorBase const *state::Effectors::getFxById(fx::ID id) const noexcept
{
  return const_cast<Effectors *>(this)->getFxById(id);
}

size_t state::Effectors::count() const noexcept
{
  return count_;
}

int state::Effectors::find(fx::EffectorBase const *fx)
{
  if (fx == 0)
  {
    return -2;
  }
  for (size_t i = 0; i < count(); ++i)
  {
    if (getFx(i) == fx)
    {
      return static_cast<int>(i);
    }
  }
  return -1;
}

fx::EffectorBase *state::Effectors::getNext(fx::EffectorBase const *cur, bool next) noexcept
{
  int ix = find(cur);
  if (ix < 0)
  {
    return getFx(0);
  }
  int d = next ? 1 : -1;
  ix = (ix + d + count()) % count();
  return getFx(ix);
}

// state::EffectParam

void state::EffectParam::read(fx::EffectorBase const *fx) noexcept
{
  id = fx->getID();
  for (size_t n = 0; n < countof(value); ++n)
  {
    gyro[n] = fx->isGyroEnabled(n);
    value[n] = fx->getParam(n);
  }
}
/// @brief エフェクターにパラメータを書き込む
/// @param[in] fx 書き込み先
void state::EffectParam::write(fx::EffectorBase *fx) const noexcept
{
  for (size_t n = 0; n < countof(value); ++n)
  {
    fx->setGyroEnable(n, gyro[n]);
    fx->setParam(n, value[n]);
  }
}

// state::Property

void state::Property::loadPatch() noexcept
{
  auto &pch = getCurrectPatch();
  for (size_t i = 0; i < countof(effectors_); ++i)
  {
    auto &param = pch.param[i];
    fx_[i] = effectors_[i].getFxById(param.id);
    param.write(fx_[i]);
  }
}

state::Property::Property(PatchTable *patch)                  //
    : bankNum_(0),                                            //
      patchNum_(0),                                           //
      effectors_({Effectors(0), Effectors(1), Effectors(2)}), //
      patches_(patch),                                        //
      editSelectedFxNum_(0)                                   //
{
  // TODO 保存データ読み出し
  factoryReset(*patch);
  loadPatch();
}

fx::EffectorBase *state::Property::getFx(uint8_t n) noexcept
{
  return fx_[n];
}

fx::EffectorBase const *state::Property::getFx(uint8_t n) const noexcept
{
  return const_cast<Property *>(this)->getFx(n);
}

fx::EffectorBase *state::Property::getEditSelectedFx() noexcept
{
  return getFx(editSelectedFxNum_);
}

fx::EffectorBase const *state::Property::getEditSelectedFx() const noexcept
{
  return const_cast<Property *>(this)->getEditSelectedFx();
}

void state::Property::changeBank(bool up) noexcept
{
  savePatch();
  if (up)
  {
    bankNum_ = (bankNum_ + 1) % MAX_BANK;
  }
  else
  {
    bankNum_ = (bankNum_ + MAX_BANK - 1) % MAX_BANK;
  }
  loadPatch();
}

void state::Property::changePatch(uint8_t n) noexcept
{
  savePatch();
  patchNum_ = n % MAX_PATCH;
  loadPatch();
}

uint8_t state::Property::getBankNum() const noexcept
{
  return bankNum_;
}

uint8_t state::Property::getPatchNum() const noexcept
{
  return patchNum_;
}

state::Patch &state::Property::getCurrectPatch() noexcept
{
  return patches_->m_[bankNum_][patchNum_];
}

void state::Property::savePatch() noexcept
{
  auto &pch = getCurrectPatch();
  for (size_t i = 0; i < countof(effectors_); ++i)
  {
    pch.param[i].read(fx_[i]);
  }
}

void state::Property::updateNextFx(bool up) noexcept
{
  auto *cur = fx_[editSelectedFxNum_];
  auto *dst = effectors_[editSelectedFxNum_].getNext(cur, up);
  dst->setDefaultParam();
  fx_[editSelectedFxNum_] = dst;
}

void state::Property::updateEditSelectedFxNum(bool up) noexcept
{
  if (up)
  {
    editSelectedFxNum_ = (editSelectedFxNum_ + 1) % MAX_EFFECTOR_COUNT;
  }
  else
  {
    editSelectedFxNum_ = (editSelectedFxNum_ + MAX_EFFECTOR_COUNT - 1) % MAX_EFFECTOR_COUNT;
  }
}

uint8_t state::Property::getEditSelectedFxNum() const noexcept
{
  return editSelectedFxNum_;
}

void state::Property::initEditSelectedFxNum() noexcept
{
  editSelectedFxNum_ = 0;
}
