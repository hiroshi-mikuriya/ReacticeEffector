/// @file      task/adc_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "common/dma_mem.h"
#include "main.h"
#include "message/type.h"
#include <algorithm>

namespace msg = satoh::msg;

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
  /// @param [in] v 更新値
  void update(uint16_t v) noexcept
  {
    min_ = std::min(v, min_);
    max_ = std::max(v, max_);
  }
  /// @brief 幅を取得
  /// @return 幅
  uint16_t getRange() const noexcept { return min_ < max_ ? max_ - min_ : 0; }
};
/// @brief 振幅をレベルに変換する
/// @param [in] v 振幅
/// @return レベル
uint8_t getLevel(uint16_t v)
{
  constexpr uint16_t th[] = {256, 362, 512, 724, 1024, 1448, 2048, 2896};
  for (uint8_t i = 0; i < 7; ++i)
  {
    if (v < th[i])
    {
      return i;
    }
  }
  return 7;
}
} // namespace

extern "C"
{
  /// @brief adcTask内部処理
  /// @param [in] argument タスク引数
  void adcTaskProc(void const *argument)
  {
    ADC_TypeDef *const adc = ADC1;
    DMA_TypeDef *const dma = DMA2;
    const uint32_t stream = LL_DMA_STREAM_4;
    auto buf = satoh::makeDmaMem<uint16_t>(2);
    LL_DMA_ConfigAddresses(dma, stream,                                             //
                           LL_ADC_DMA_GetRegAddr(adc, LL_ADC_DMA_REG_REGULAR_DATA), //
                           reinterpret_cast<uint32_t>(buf.get()),                   //
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY                        //
    );
    LL_DMA_SetDataLength(dma, stream, sizeof(buf.get()) / 2);
    LL_DMA_EnableIT_TC(dma, stream);
    LL_DMA_EnableIT_TE(dma, stream);
    LL_DMA_EnableStream(dma, stream);
    LL_ADC_Enable(adc);
    LL_ADC_REG_StartConversionSWStart(adc);
    RangeMeter left, right;
    for (;;)
    {
      osEvent ev = osSignalWait(SIG_DMAEND | SIG_DMAERR | SIG_TIMER, osWaitForever);
      if (ev.value.signals & SIG_DMAEND)
      {
        left.update(buf.get()[0]);
        right.update(buf.get()[1]);
      }
      if (ev.value.signals & SIG_TIMER)
      {
        msg::LED_LEVEL level{};
        level.left = getLevel(left.getRange());
        level.right = getLevel(right.getRange());
        msg::send(i2cTaskHandle, msg::LED_LEVEL_UPDATE_REQ, level);
        msg::NEO_PIXEL_SPEED speed = {static_cast<uint32_t>(300 / (level.right + 1))};
        msg::send(neoPixelTaskHandle, msg::NEO_PIXEL_SET_SPEED, speed);
        left.reset();
        right.reset();
      }
    }
  }
  /// @brief ADC変換完了DMA割り込み
  void adc1CpltIRQ(void) { osSignalSet(adcTaskHandle, SIG_DMAEND); }
  /// @brief ADC変換エラーDMA割り込み
  void adc1ErrorIRQ(void) { osSignalSet(adcTaskHandle, SIG_DMAERR); }
  /// @brief ADC TIM割り込み
  void adc1TimIRQ(void) { osSignalSet(adcTaskHandle, SIG_TIMER); }
} // extern "C"