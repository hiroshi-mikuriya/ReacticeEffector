/// @file      task/i2c_monitor_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "main.h"
#include "message/type.h"

namespace
{
constexpr int32_t SIG_MODE_KEY = 1 << 16;
constexpr int32_t SIG_ENCODER = 1 << 17;
} // namespace

extern "C"
{
  /// @brief i2cMonitorTask内部処理
  /// @param [in] argument タスク引数
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
  /// @brief GPIO_IN_INT_SW_N 割り込み
  void notifyExtiSwIRQ(void) { osSignalSet(i2cMonitorTaskHandle, SIG_ENCODER); }
  /// @brief GPIO_IN_INT_SW2_N 割り込み
  void notifyExtiSw2IRQ(void) { osSignalSet(i2cMonitorTaskHandle, SIG_MODE_KEY); }
} // extern "C"