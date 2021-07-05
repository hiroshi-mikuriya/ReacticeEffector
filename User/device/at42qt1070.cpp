/// @file      device/at42qt1070.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "at42qt1070.h"

namespace
{
constexpr uint8_t SLAVE_ADDR = 0x1B;
constexpr uint8_t DETECTION_STATUS = 0x02;
constexpr uint8_t KEY_STATUS = 0x03;
constexpr uint8_t AVE_AKS_KEY_0 = 0x27;
constexpr uint8_t AVE_AKS_KEY_1 = 0x28;
constexpr uint8_t AVE_AKS_KEY_2 = 0x29;
constexpr uint8_t AVE_AKS_KEY_3 = 0x2A;
constexpr uint8_t AVE_AKS_KEY_4 = 0x2B;
constexpr uint8_t AVE_AKS_KEY_5 = 0x2C;
constexpr uint8_t AVE_AKS_KEY_6 = 0x2D;
constexpr uint8_t FO_MO_GUARD_NO = 0x35;
} // namespace

bool sleep()
{
  osDelay(1);
  return true;
}

bool satoh::AT42QT1070::init() const noexcept
{
  uint8_t v0[] = {AVE_AKS_KEY_6, 0};
  uint8_t v1[] = {FO_MO_GUARD_NO, 0};
  uint8_t buf[2] = {0};
  bool res = true                                                 //
             && write(v0, sizeof(v0))                             //
             && write(v1, sizeof(v1))                             //
             && sleep()                                           //
             && I2CDeviceBase::read(KEY_STATUS, buf, sizeof(buf)) //
      ;
  if (res)
  {
    cache_ = buf[1];
  }
  return res;
}
satoh::AT42QT1070::AT42QT1070(I2C *i2c) noexcept //
    : I2CDeviceBase(i2c, SLAVE_ADDR),            //
      ok_(init())                                //
{
}
satoh::AT42QT1070::~AT42QT1070() {}
bool satoh::AT42QT1070::ok() const noexcept
{
  return ok_;
}
int satoh::AT42QT1070::read(bool (&keys)[6]) const noexcept
{
  uint8_t v[2] = {0};
  // DETECTION_STATUSを読み出さないと次回の割り込みが発生しなくなる
  if (!I2CDeviceBase::read(DETECTION_STATUS, v, sizeof(v)))
  {
    return -1;
  }
  if (v[1] == cache_)
  {
    return 0;
  }
  for (int i = 0; i < 6; ++i)
  {
    uint8_t bit = 1 << i;
    keys[i] = !!(v[1] & bit);
  }
  cache_ = v[1];
  return 1;
}
