/// @file      task/neo_pixel_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "cmsis_os.h"
#include "common/alloc.hpp"
#include "device/neo_pixel.h"
#include "main.h"
#include "message/type.h"

namespace msg = satoh::msg;

namespace
{
satoh::SpiMaster *s_spi = 0;
} // namespace

extern "C"
{
  /// @brief neoPixelTask内部処理
  /// @param [in] argument タスク引数
  void neoPixelTaskProc(void const *argument)
  {
    if (msg::registerThread(2) != osOK)
    {
      return;
    }
    constexpr uint32_t LED_COUNT = 100;
    s_spi = satoh::alloc<satoh::SpiMaster>(NP_SPI, DMA1, LL_DMA_STREAM_5);
    satoh::NeoPixel np(s_spi, LED_COUNT);
    msg::NEO_PIXEL_PATTERN ptn{};
    msg::NEO_PIXEL_SPEED speed = {100};
    for (int i = 0;; i = (i + 1) % 6)
    {
      auto res = msg::recv(speed.interval);
      if (res.status() == osOK && res.msg())
      {
        auto msg = res.msg();
        if (msg->type == msg::NEO_PIXEL_SET_SPEED)
        {
          speed = *reinterpret_cast<msg::NEO_PIXEL_SPEED const *>(msg->bytes);
        }
        if (msg->type == msg::NEO_PIXEL_SET_PATTERN)
        {
          ptn = *reinterpret_cast<msg::NEO_PIXEL_PATTERN const *>(msg->bytes);
        }
        continue;
      }
      np.clear();
      for (uint32_t n = 0; n < LED_COUNT; ++n)
      {
        np.set(ptn.rgb[(n + i) % 6], n);
      }
      np.show();
    }
  }
  /// @brief NeoPixel DMA送信完了割り込み
  void neoPixelTxEndIRQ(void) { s_spi->notifyTxEndIRQ(); }
  /// @brief NeoPixel DMA送信エラー割り込み
  void neoPixelTxErrorIRQ(void) { s_spi->notifyTxErrorIRQ(); }
} // extern "C"