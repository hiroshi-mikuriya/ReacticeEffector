/// @file      task/i2c_monitor_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_monitor_task.h"
#include "message/msglib.h"
#include "task/i2c_task.h"

namespace
{
constexpr int32_t SIG_MODE_KEY = 1 << 16;
constexpr int32_t SIG_ENCODER = 1 << 17;
} // namespace

void i2cMonitorTaskProc(void const *argument)
{
  for (;;)
  {
    osEvent ev = osSignalWait(SIG_MODE_KEY | SIG_ENCODER, 50);
    if (ev.status == osEventTimeout)
    {
      // ロータリーエンコーダとモードキーの割り込みは発生しないことがあるので、定期的に読み出す
      satoh::msg::send(i2cTaskHandle, satoh::msg::ENCODER_GET_REQ);
      satoh::msg::send(i2cTaskHandle, satoh::msg::MODE_KEY_GET_REQ);
    }
  }
}

void notifyExtiSwIRQ(void)
{
  osSignalSet(i2cMonitorTaskHandle, SIG_ENCODER);
}

void notifyExtiSw2IRQ(void)
{
  osSignalSet(i2cMonitorTaskHandle, SIG_MODE_KEY);
}
