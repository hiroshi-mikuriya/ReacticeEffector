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

void appTaskProc(void const *argument)
{
  if (satoh::registerMsgTarget(4) != osOK)
  {
    return;
  }
  satoh::AllocCls<satoh::state::PatchTable> patch; // TODO 保存から読み出す or 初期値を入れる
  satoh::AllocCls<satoh::state::Property> prop(patch.get());
  satoh::state::PatchEdit stPE(*prop);
  satoh::state::EffectEdit stEE(*prop);
  satoh::state::Playing stPL(*prop);
  satoh::state::Error stER(*prop);
  satoh::state::Base *states[] = {&stPL, &stPE, &stEE, &stER};
  satoh::state::ID stID = satoh::state::PLAYING;
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