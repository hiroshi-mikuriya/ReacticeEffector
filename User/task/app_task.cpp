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
  satoh::state::PatchEdit stPE(s_prop);
  satoh::state::EffectEdit stEE(s_prop);
  satoh::state::Playing stPL(s_prop);
  satoh::state::Error stER(s_prop);
  satoh::state::Base *states[] = {&stPL, &stPE, &stEE, &stER};
  satoh::state::ID stID = satoh::state::PLAYING;
  // TODO 暫定的にパッチに初期値を入れておく
  s_prop.patches[0][0].fx[0] = s_prop.effectors[0].getFx(0);
  s_prop.patches[0][1].fx[0] = s_prop.effectors[0].getFx(1);
  s_prop.patches[0][2].fx[0] = s_prop.effectors[0].getFx(2);
  s_prop.patches[0][3].fx[0] = s_prop.effectors[0].getFx(3);

  s_prop.patches[1][0].fx[0] = s_prop.effectors[0].getFx(4);
  s_prop.patches[1][1].fx[0] = s_prop.effectors[0].getFx(5);
  s_prop.patches[1][2].fx[0] = s_prop.effectors[0].getFx(6);
  s_prop.patches[1][3].fx[0] = s_prop.effectors[0].getFx(7);

  s_prop.patches[2][0].fx[0] = s_prop.effectors[0].getFx(8);
  s_prop.patches[2][1].fx[0] = s_prop.effectors[0].getFx(1);
  s_prop.patches[2][1].fx[1] = s_prop.effectors[1].getFx(2);
  s_prop.patches[2][2].fx[0] = s_prop.effectors[0].getFx(1);
  s_prop.patches[2][2].fx[1] = s_prop.effectors[1].getFx(3);
  s_prop.patches[2][3].fx[0] = s_prop.effectors[0].getFx(3);
  s_prop.patches[2][3].fx[1] = s_prop.effectors[1].getFx(4);
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