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

namespace
{
/// 状態プロパティ
satoh::state::Property s_prop;
} // namespace

void appTaskProc(void const *argument)
{
  if (satoh::registerMsgTarget(4) != osOK)
  {
    return;
  }
  satoh::state::PatchEdit stPE;
  satoh::state::EffectEdit stEE;
  satoh::state::Playing stPL;
  satoh::state::Error stER;
  satoh::state::Base *states[] = {&stPL, &stPE, &stEE, &stER};
  satoh::state::ID stID = satoh::state::PLAYING;
  // TODO 暫定的にパッチに初期値を入れておく
  s_prop.patches[0][0].fx[0] = s_prop.effectors[0].list[0];
  s_prop.patches[0][1].fx[0] = s_prop.effectors[0].list[1];
  s_prop.patches[0][2].fx[0] = s_prop.effectors[0].list[2];
  s_prop.patches[0][3].fx[0] = s_prop.effectors[0].list[3];

  s_prop.patches[1][0].fx[0] = s_prop.effectors[0].list[4];
  s_prop.patches[1][1].fx[0] = s_prop.effectors[0].list[5];
  s_prop.patches[1][2].fx[0] = s_prop.effectors[0].list[6];
  s_prop.patches[1][3].fx[0] = s_prop.effectors[0].list[7];

  s_prop.patches[2][0].fx[0] = s_prop.effectors[0].list[8];
  s_prop.patches[2][1].fx[0] = s_prop.effectors[0].list[1];
  s_prop.patches[2][1].fx[1] = s_prop.effectors[0].list[2];
  s_prop.patches[2][2].fx[0] = s_prop.effectors[0].list[1];
  s_prop.patches[2][2].fx[1] = s_prop.effectors[0].list[3];
  s_prop.patches[2][3].fx[0] = s_prop.effectors[0].list[3];
  s_prop.patches[2][3].fx[1] = s_prop.effectors[0].list[4];
  states[stID]->init(s_prop);
  for (;;)
  {
    auto res = satoh::recvMsg();
    auto *msg = res.msg();
    if (msg == 0)
    {
      continue;
    }
    auto next = states[stID]->run(msg, s_prop);
    if (stID != next)
    {
      states[stID]->deinit(s_prop);
      stID = next;
      states[stID]->init(s_prop);
    }
  }
}