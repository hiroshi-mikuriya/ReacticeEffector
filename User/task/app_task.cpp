/// @file      task/app_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "task/app_task.h"
#include "app_state/effect_edit.h"
#include "app_state/error.h"
#include "app_state/patch_edit.h"
#include "app_state/playing.h"
#include "common/alloc.hpp"
#include "stm32f7xx.h"
#include "stm32f7xx_ll_pwr.h"
#include <memory>

namespace
{
/// @brief バックアップ機能初期化
void initBackup()
{
  __HAL_RCC_PWR_CLK_ENABLE();
  LL_PWR_EnableBkUpAccess();
  __HAL_RCC_BKPSRAM_CLK_ENABLE();
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
  std::unique_ptr<state::Property, satoh::Deleter<state::Property>> prop(satoh::alloc<state::Property>(patch));
  state::PatchEdit stPE(*prop);
  state::EffectEdit stEE(*prop);
  state::Playing stPL(*prop);
  state::Error stER(*prop);
  state::Base *states[] = {&stPL, &stPE, &stEE, &stER};
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