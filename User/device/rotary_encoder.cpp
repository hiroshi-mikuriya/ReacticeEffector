/// @file      device/rotary_encoder.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "rotary_encoder.h"
#include "message/type.h"
#include "pca9555.h"
#include <cstring> // memset

bool satoh::RotaryEncoder::init() noexcept
{
  osDelay(1);
  return I2CDeviceBase::read(pca9555::INPUT_0, cache_, sizeof(cache_));
}

satoh::RotaryEncoder::RotaryEncoder(I2C *i2c) noexcept //
    : I2CDeviceBase(i2c, pca9555::ROTARY_ENCODER),     //
      ok_(init())                                      //
{
}

satoh::RotaryEncoder::~RotaryEncoder() {}

bool satoh::RotaryEncoder::ok() const noexcept
{
  return ok_;
}

int satoh::RotaryEncoder::read(uint8_t (&button)[4], int8_t (&angleDiff)[4]) noexcept
{
  uint8_t buf[2] = {0};
  if (!I2CDeviceBase::read(pca9555::INPUT_0, buf, sizeof(buf)))
  {
    return -1;
  }
  int res = 0;
  if (cache_[0] == buf[0] && cache_[1] == buf[1])
  {
    return res;
  }
  for (int i = 0; i < 4; ++i)
  {
    // ロータリーエンコーダの確認
    uint8_t abit = 0b0000'0001 << (i * 2);
    uint8_t bbit = 0b0000'0010 << (i * 2);
    // bool a0 = !!(cache_[0] & abit);
    bool b0 = !!(cache_[0] & bbit);
    bool a1 = !!(buf[0] & abit);
    bool b1 = !!(buf[0] & bbit);
    if (!b0 && b1) // B相立ち上がりエッジ
    {
      angleDiff[i] = a1 ? 1 : -1;
      res |= 1;
    }
    else
    {
      angleDiff[i] = 0;
    }
    // ボタン押下の確認
    uint8_t bit = 1 << i;
    button[i] = (buf[1] & bit) ? msg::BUTTON_UP : msg::BUTTON_DOWN;
  }
  if (cache_[1] != buf[1])
  {
    res |= 2;
  }
  cache_[0] = buf[0];
  cache_[1] = buf[1];
  return res;
}
