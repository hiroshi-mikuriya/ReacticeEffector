/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "common/alloc.hpp"
#include "handles.h"
#include "main.h"
#include "state/effect_edit.h"
#include "state/error.h"
#include "state/factory_reset.h"
#include "state/patch_edit.h"
#include "state/playing.h"
#include "state/tuner.h"

namespace state = satoh::state;
namespace msg = satoh::msg;

namespace
{
/// SPI SRAMと通信するオブジェクト
satoh::SpiMaster *s_spi = 0;
/// @brief バックアップ機能初期化
void initBackup()
{
  LL_AHB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_BKPSRAM);
  LL_PWR_EnableBkUpRegulator();
  while (!LL_PWR_IsActiveFlag_BRR())
  {
    osDelay(1);
  }
}
} // namespace

extern "C"
{
  /// @brief appTask内部処理
  /// @param [in] argument 未使用
  void appTaskProc(void const *argument)
  {
    UNUSED(argument);
    s_spi = satoh::alloc<satoh::SpiMaster>(SRAM_SPI, DMA1, LL_DMA_STREAM_4, LL_DMA_STREAM_3, SRAM_SPI_NSS_GPIO_Port, SRAM_SPI_NSS_Pin);
    if (msg::registerThread(4) != osOK)
    {
      return;
    }
    initBackup();
    auto *patch = reinterpret_cast<state::PatchTable *>(BKPSRAM_BASE);
    satoh::UniquePtr<state::Property> prop(satoh::alloc<state::Property>(patch, s_spi));
    state::PatchEdit stPE(*prop);
    state::EffectEdit stEE(*prop);
    state::Playing stPL(*prop);
    state::FactoryReset stFR(*prop);
    state::Tuner stTN(*prop);
    state::Error stER(*prop);
    state::Base *states[] = {&stPL, &stPE, &stEE, &stFR, &stTN, &stER};
    state::ID stID = state::PLAYING;
    states[stID]->init();

    LL_TIM_EnableCounter(TIM10);
    LL_TIM_EnableIT_UPDATE(TIM10);
    for (;;)
    {
      auto res = msg::recv();
      auto *msg = res.msg();
      if (msg == 0)
      {
        continue;
      }
      auto next = states[stID]->run(msg);
      if (stID != next)
      {
        states[stID]->deinit();
        stID = next;
        states[stID]->init();
      }
    }
  }
  /// @brief APP TIM割り込み
  void appTimIRQ(void) { msg::send(appTaskHandle, msg::APP_TIM_NOTIFY); }
  /// @brief SPI SRAM送信完了割り込み
  void spiSramTxEndIRQ(void) { s_spi->notifyTxEndIRQ(); }
  /// @brief SPI SRAM送信エラー割り込み
  void spiSramTxErrorIRQ(void) { s_spi->notifyTxErrorIRQ(); }
  /// @brief SPI SRAM受信完了割り込み
  void spiSramRxEndIRQ(void) { s_spi->notifyRxEndIRQ(); }
  /// @brief SPI SRAM受信エラー割り込み
  void spiSramRxErrorIRQ(void) { s_spi->notifyRxErrorIRQ(); }
} // extern "C"