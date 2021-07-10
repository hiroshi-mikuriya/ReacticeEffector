/// @file      task/adc_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/adc_task.h"
#include "message/msglib.h"
#include "stm32f7xx_ll_adc.h"
#include "stm32f7xx_ll_dma.h"
#include "task/i2c_task.h"
#include <algorithm>

namespace
{
constexpr int32_t SIG_DMAEND = 1 << 0;
constexpr int32_t SIG_DMAERR = 1 << 1;
constexpr int32_t SIG_TIMER = 1 << 2;

/// @brief 幅測定クラス
class RangeMeter
{
  uint16_t min_; ///< 下限値
  uint16_t max_; ///< 上限値

public:
  /// @brief コンストラクタ
  RangeMeter() { reset(); }
  /// @brief 幅を初期化
  void reset() noexcept
  {
    min_ = 0xFFFF;
    max_ = 0;
  }
  /// @brief 上限・下限を更新
  /// @param[in] v 更新値
  void update(uint16_t v) noexcept
  {
    min_ = std::min(v, min_);
    max_ = std::max(v, max_);
  }
  /// @brief 幅を取得
  /// @return 幅
  uint16_t getRange() const noexcept { return max_ - min_; }
};
/// @brief 振幅をレベルに変換する
/// @param[in] v 振幅
/// @return レベル
uint8_t getLevel(uint16_t v)
{
  constexpr uint16_t th[] = {181, 256, 362, 512, 724, 1024, 1448, 2048};
  for (uint8_t i = 0; i < 7; ++i)
  {
    if (v < th[i])
    {
      return i;
    }
  }
  return 7;
}
/// @brief サンプリング結果からレベル値を計算する通知
void notifySamplingEnd(void const *arg)
{
  osSignalSet(adcTaskHandle, SIG_TIMER);
}
} // namespace

void adcTaskProc(void const *argument)
{
  ADC_TypeDef *const adc = ADC1;
  DMA_TypeDef *const dma = DMA2;
  const uint32_t stream = LL_DMA_STREAM_4;
  uint16_t buf[2] = {};
  LL_DMA_ConfigAddresses(dma, stream,                                             //
                         LL_ADC_DMA_GetRegAddr(adc, LL_ADC_DMA_REG_REGULAR_DATA), //
                         reinterpret_cast<uint32_t>(buf),                         //
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY                        //
  );
  LL_DMA_SetDataLength(dma, stream, sizeof(buf) / 2);
  LL_DMA_EnableIT_TC(dma, stream);
  LL_DMA_EnableIT_TE(dma, stream);
  LL_DMA_EnableStream(dma, stream);
  LL_ADC_Enable(adc);
  LL_ADC_REG_StartConversionSWStart(adc);
  constexpr uint32_t FREQ = 50;
  constexpr uint32_t INTERVAL = 1000 / FREQ;
  osTimerDef(timer, notifySamplingEnd);
  osTimerId tid = osTimerCreate(osTimer(timer), osTimerPeriodic, 0);
  osTimerStart(tid, INTERVAL);
  RangeMeter left, right;
  for (;;)
  {
    osEvent ev = osSignalWait(SIG_DMAEND | SIG_DMAERR | SIG_TIMER, osWaitForever);
    if (ev.value.signals & SIG_DMAEND)
    {
      left.update(buf[0]);
      right.update(buf[1]);
    }
    if (ev.value.signals & SIG_TIMER)
    {
      satoh::msg::LED_LEVEL level{};
      level.left = getLevel(left.getRange());
      level.right = getLevel(right.getRange());
      satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_LEVEL_UPDATE_REQ, &level, sizeof(level));
      left.reset();
      right.reset();
    }
  }
}

void adc1CpltIRQ(void)
{
  osSignalSet(adcTaskHandle, SIG_DMAEND);
}

void adc1ErrorIRQ(void)
{
  osSignalSet(adcTaskHandle, SIG_DMAERR);
}
