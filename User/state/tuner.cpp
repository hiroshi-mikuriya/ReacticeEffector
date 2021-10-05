/// @file      state/tuner.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "tuner.h"
#include "common.h"
#include "common/utils.h"
#include "user.h"

namespace msg = satoh::msg;
namespace state = satoh::state;

namespace
{
/// @brief 音定義型
struct Tone
{
  float freq;       ///< 音基準値
  float maxFreq;    ///< 最大周波数
  char const *name; ///< 音名
};
/// @brief 音定義
constexpr Tone TONES[] = {
    {38.891f, 40.0305f, "D#"},   //
    {41.203f, 42.4103f, "E"},    //
    {43.654f, 44.9331f, "F"},    //
    {46.249f, 47.6041f, "F#"},   //
    {48.999f, 50.4347f, "G"},    //
    {51.913f, 53.4341f, "G#"},   //
    {55.0f, 56.6116f, "A"},      //
    {58.27f, 59.9774f, "A#"},    //
    {61.735f, 63.5439f, "B"},    //
    {65.406f, 67.3225f, "C"},    //
    {69.296f, 71.3265f, "C#"},   //
    {73.416f, 75.5672f, "D"},    //
    {77.782f, 80.0611f, "D#"},   //
    {82.407f, 84.8217f, "E"},    //
    {87.307f, 89.8652f, "F"},    //
    {92.499f, 95.2094f, "F#"},   //
    {97.999f, 100.8705f, "G"},   //
    {103.826f, 106.8683f, "G#"}, //
    {110.0f, 113.2232f, "A"},    //
};

/// @brief 音名・ずれを分析する @param[in] freq 周波数 @return 分析結果
msg::OLED_DISP_TUNER analyze(float freq)
{
  msg::OLED_DISP_TUNER dst{};
  dst.estimated = true;
  dst.freq = freq;
  while (110 < freq)
  {
    freq /= 2;
  }
  for (auto const &t : TONES)
  {
    if (freq < t.maxFreq)
    {
      strcpy(dst.name, t.name);
      for (int i = -7; i <= 7; ++i)
      {
        float th = t.freq * std::pow(1.00828f, i);
        if (freq < th)
        {
          dst.diff = i;
          break;
        }
      }
      break;
    }
  }
  return dst;
}
} // namespace

state::ID state::Tuner::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == msg::BUTTON_DOWN)
  {
    return PLAYING;
  }
  if (src->rtn == msg::BUTTON_DOWN)
  {
    return PLAYING;
  }
  if (src->up == msg::BUTTON_DOWN)
  {
  }
  if (src->down == msg::BUTTON_DOWN)
  {
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
state::ID state::Tuner::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
    if (src->button[i] == msg::BUTTON_DOWN)
    {
    }
  }
  return id();
}
state::ID state::Tuner::run(msg::ACC_GYRO const *src) noexcept
{
  proc(m_, src);
  return id();
}
state::ID state::Tuner::run(msg::ROTARY_ENCODER const *src) noexcept
{
  for (auto angle : src->angleDiff)
  {
    if (0 < angle)
    {
    }
    if (angle < 0)
    {
    }
  }
  return id();
}
state::ID state::Tuner::run(msg::ERROR const *src) noexcept
{
  m_.setError(src->cause);
  return ERROR;
}
state::ID state::Tuner::timer() noexcept
{
  fx::Tuner *tuner = m_.getTuner();
  msg::OLED_DISP_TUNER cmd{};
  if (osKernelSysTick() - tuner->getLastUpdateTime() < 500)
  {
    cmd = analyze(tuner->getEstimatedFreq());
  }
  msg::send(i2cTaskHandle, msg::OLED_DISP_TUNER_REQ, cmd);
  return id();
}
void state::Tuner::init() noexcept
{
  {
    msg::LED_ALL_EFFECT cmd{};
    msg::send(i2cTaskHandle, msg::LED_ALL_EFFECT_REQ, cmd);
  }
  {
    msg::SOUND_EFFECTOR cmd{};
    cmd.fx[0] = m_.getTuner();
    msg::send(soundTaskHandle, msg::SOUND_CHANGE_EFFECTOR_REQ, cmd);
  }
}
void state::Tuner::deinit() noexcept {}
