/// @file      state/property.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "property.h"
#include "common/utils.h"
#include "effector/booster.hpp"
#include "effector/bq_filter.hpp"
#include "effector/bypass.hpp"
#include "effector/chorus.hpp"
#include "effector/delay.hpp"
#include "effector/distortion.hpp"
#include "effector/oscillator.hpp"
#include "effector/overdrive.hpp"
#include "effector/phaser.hpp"
#include "effector/reverb.hpp"
#include "effector/tremolo.hpp"
#include "factory_reset.h"
#include "stm32f7xx.h"
#include "stm32f7xx_ll_crc.h"

namespace fx = satoh::fx;
namespace state = satoh::state;

// state::Effectors

state::Effectors::Effectors(uint8_t n, SpiMaster *spi) : count_(0)
{
  addList<fx::Bypass>(true);
  addList<fx::Booster>(true);
  addList<fx::OverDrive>(true);
  addList<fx::Distortion>(true);
  addList<fx::Chorus>(true);
  addList<fx::Phaser>(true);
  addList<fx::Tremolo>(true);
  addList<fx::Delay>(0 < n);
  addList<fx::Oscillator>(n == 0);
  addList<fx::BqFilter>(true);
  addList<fx::Reverb>(n == 2);
}

fx::EffectorBase *state::Effectors::getFx(size_t i) noexcept
{
  return list_[i % count()].get();
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

void state::EffectParam::write(fx::EffectorBase *fx) const noexcept
{
  for (size_t n = 0; n < countof(value); ++n)
  {
    fx->setGyroEnable(n, gyro[n]);
    fx->setParam(n, value[n]);
  }
}

uint32_t state::PatchTable::calcCrc() const noexcept
{
  LL_CRC_SetInitialData(CRC, 0);
  auto p = reinterpret_cast<uint32_t const *>(m_);
  for (size_t i = 0; i < sizeof(m_) / sizeof(*p); ++i)
  {
    LL_CRC_FeedData32(CRC, *(p++));
  }
  return LL_CRC_ReadData32(CRC);
}

// state::Tap

state::Tap::Tap() : lastTapTime_(0), tapInterval_(0) {}
state::Tap::~Tap() {}
void state::Tap::notifyTapEvent() noexcept
{
  uint32_t now = osKernelSysTick();
  if (lastTapTime_ != 0)
  {
    tapInterval_ = now - lastTapTime_;
  }
  lastTapTime_ = now;
}
uint32_t state::Tap::getTapInterval() const noexcept
{
  return tapInterval_;
}
bool state::Tap::getLedLevel() const noexcept
{
  if (lastTapTime_ == 0 || tapInterval_ == 0)
  {
    return false;
  }
  uint32_t d = osKernelSysTick() - lastTapTime_;
  uint32_t a = d % tapInterval_;
  return a < 100;
}
void state::Tap::reset() noexcept
{
  lastTapTime_ = tapInterval_ = 0;
}

// state::Property

void state::Property::loadPatch() noexcept
{
  auto const &pch = getCurrectPatch();
  for (size_t i = 0; i < countof(effectors_); ++i)
  {
    auto &param = pch.param[i];
    fx_[i] = effectors_[i].getFxById(param.id);
    param.write(fx_[i]);
  }
}

state::Property::Property(PatchTable *patch, SpiMaster *spi) //
    : bankNum_(0),                                           //
      patchNum_(0),                                          //
      effectors_({Effectors(0, spi),                         //
                  Effectors(1, spi),                         //
                  Effectors(2, spi)}),                       //
      patches_(patch),                                       //
      editSelectedFxNum_(0),                                 //
      error_(msg::error::NONE)                               //
{
  if (patch->calcCrc() != patch->crc_)
  {
    factoryReset();
  }
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

state::Patch const &state::Property::getCurrectPatch() const noexcept
{
  return const_cast<Property *>(this)->getCurrectPatch();
}

satoh::RGB state::Property::getCurrentColor() const noexcept
{
  for (auto const *fx : fx_)
  {
    if (fx->getID() != fx::BYPASS)
    {
      return fx->getColor();
    }
  }
  return RGB{0x20, 0x20, 0x20};
}

void state::Property::savePatch() noexcept
{
  auto &pch = getCurrectPatch();
  for (size_t i = 0; i < countof(effectors_); ++i)
  {
    pch.param[i].read(fx_[i]);
  }
  patches_->crc_ = patches_->calcCrc();
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

void state::Property::factoryReset() noexcept
{
  patches_->m_[0][0] = Patch{{
      {fx::BYPASS}, //
      {fx::BYPASS}, //
      {fx::BYPASS}, //
  }};
  patches_->m_[0][1] = Patch{{
      {fx::BOOSTER, {}, {5, 5, 5}}, //
      {fx::BYPASS},                 //
      {fx::BYPASS},                 //
  }};
  patches_->m_[0][2] = Patch{{
      {fx::OVERDRIVE, {}, {60, 75, 50, 50}},      //
      {fx::BYPASS},                               //
      {fx::REVERB, {}, {50, 50, 60, 50, 50, 50}}, //
  }};
  patches_->m_[0][3] = Patch{{
      {fx::DISTORTION, {}, {75, 75, 50}}, //
      {fx::BYPASS},                       //
      {fx::BYPASS},                       //
  }};
  patches_->m_[1][0] = Patch{{
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::BYPASS},                               //
      {fx::BYPASS},                               //
  }};
  patches_->m_[1][1] = Patch{{
      {fx::TREMOLO, {}, {50, 50, 50, 50}}, //
      {fx::BYPASS},                        //
      {fx::BYPASS},                        //
  }};
  patches_->m_[1][2] = Patch{{
      {fx::PHASER, {}, {50, 50, 3}}, //
      {fx::BYPASS},                  //
      {fx::BYPASS},                  //
  }};
  patches_->m_[1][3] = Patch{{
      {fx::BYPASS},                       //
      {fx::DELAY, {}, {500, 50, 50, 50}}, //
      {fx::BYPASS},                       //
  }};
  patches_->m_[2][0] = Patch{{
      {fx::OSCILLATOR, {false, true, false}, {50, 100, 1}}, //
      {fx::BYPASS},                                         //
      {fx::BYPASS},                                         //
  }};
  patches_->m_[2][1] = Patch{{
      {fx::BOOSTER, {}, {7, 5, 5}},          //
      {fx::OVERDRIVE, {}, {75, 75, 50, 50}}, //
      {fx::BYPASS},                          //
  }};
  patches_->m_[2][2] = Patch{{
      {fx::BOOSTER, {}, {7, 5, 5}},       //
      {fx::DISTORTION, {}, {75, 75, 50}}, //
      {fx::BYPASS},                       //
  }};
  patches_->m_[2][3] = Patch{{
      {fx::OVERDRIVE, {}, {60, 75, 50, 50}},      //
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::BYPASS},                               //
  }};
  patches_->m_[3][0] = Patch{{
      {fx::DISTORTION, {}, {75, 75, 50}},         //
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::DELAY, {}, {200, 30, 49, 50}},         //
  }};
  patches_->m_[3][1] = Patch{{
      {fx::OVERDRIVE, {}, {75, 75, 50, 50}}, //
      {fx::PHASER, {}, {50, 50, 3}},         //
      {fx::BYPASS},                          //
  }};
  patches_->m_[3][2] = Patch{{
      {fx::TREMOLO, {}, {50, 50, 50, 50}},        //
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::BYPASS},                               //
  }};
  patches_->m_[3][3] = Patch{{
      {fx::OVERDRIVE, {}, {50, 50, 50, 50}}, //
      {fx::DELAY, {}, {320, 50, 50, 50}},    //
      {fx::DELAY, {}, {890, 50, 50, 50}},    //
  }};
  patches_->crc_ = patches_->calcCrc();
  loadPatch();
}
