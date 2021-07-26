/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/app_task.h"
#include "app_state/effect_edit.h"
#include "app_state/error.h"
#include "app_state/factory_reset.h"
#include "app_state/patch_edit.h"
#include "app_state/playing.h"
#include "common/alloc.hpp"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_pwr.h"
#include <memory>

namespace
{
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

void appTaskProc(void const *argument)
{
  namespace state = satoh::state;
  if (satoh::registerMsgTarget(4) != osOK)
  {
    return;
  }
  initBackup();
  auto *patch = reinterpret_cast<state::PatchTable *>(BKPSRAM_BASE);
  satoh::UniquePtr<state::Property> prop(satoh::alloc<state::Property>(patch));
  state::PatchEdit stPE(*prop);
  state::EffectEdit stEE(*prop);
  state::Playing stPL(*prop);
  state::FactoryReset stFR(*prop);
  state::Error stER(*prop);
  state::Base *states[] = {&stPL, &stPE, &stEE, &stFR, &stER};
  state::ID stID = state::PLAYING;
  states[stID]->init();
  for (;;)
  {
    auto res = satoh::recvMsg();
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