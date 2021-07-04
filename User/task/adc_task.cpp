/// @file      task/adc_tasl.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/adc_task.h"
#include "message/msglib.h"
#include "stm32f7xx_ll_adc.h"
#include "task/i2c_task.h"
#include <algorithm>

namespace
{
constexpr int32_t SIG_ADC = 1 << 0;

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
} // namespace

void adcTaskProc(void const *argument)
{
  LL_ADC_EnableIT_EOCS(ADC1);
  LL_ADC_Enable(ADC1);
  LL_ADC_REG_StartConversionSWStart(ADC1);
  constexpr uint32_t FREQ = 50;
  constexpr uint32_t INTERVAL = 1000 / FREQ;
  uint32_t lastTxTick = HAL_GetTick();
  RangeMeter meter;
  for (;;)
  {
    osSignalWait(SIG_ADC, osWaitForever);
    uint16_t v = LL_ADC_REG_ReadConversionData12(ADC1);
    meter.update(v);
    uint32_t now = HAL_GetTick();
    if (INTERVAL <= now - lastTxTick)
    {
      uint16_t power = meter.getRange();
      meter.reset();
      lastTxTick = now;
      satoh::msg::LED_LEVEL level{};
      level.left = static_cast<uint8_t>(std::min(power / 60, 7));
      level.right = static_cast<uint8_t>(std::min(power / 60, 7));
      satoh::sendMsg(i2cTaskHandle, satoh::msg::LED_LEVEL_UPDATE_REQ, &level, sizeof(level));
    }
  }
}

void adc1IRQ(void)
{
  if (LL_ADC_IsActiveFlag_EOCS(ADC1))
  {
    LL_ADC_ClearFlag_EOCS(ADC1);
    osSignalSet(adcTaskHandle, SIG_ADC);
  }
}