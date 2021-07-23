/// @file      task/app_state/factory_reset.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "factory_reset.h"

namespace state = satoh::state;

void state::factoryReset(state::PatchTable &pch) noexcept
{
  pch.m_[0][0] = Patch{{
      {fx::BYPASS, {}, {}}, //
      {fx::BYPASS, {}, {}}, //
      {fx::BYPASS, {}, {}}, //
  }};
  pch.m_[0][1] = Patch{{
      {fx::BOOSTER, {}, {5, 5, 5}}, //
      {fx::BYPASS, {}, {}},         //
      {fx::BYPASS, {}, {}},         //
  }};
  pch.m_[0][2] = Patch{{
      {fx::OVERDRIVE, {}, {75, 75, 50, 50}}, //
      {fx::BYPASS, {}, {}},                  //
      {fx::BYPASS, {}, {}},                  //
  }};
  pch.m_[0][3] = Patch{{
      {fx::DISTORTION, {}, {75, 75, 50}}, //
      {fx::BYPASS, {}, {}},               //
      {fx::BYPASS, {}, {}},               //
  }};
  pch.m_[1][0] = Patch{{
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::BYPASS, {}, {}},                       //
      {fx::BYPASS, {}, {}},                       //
  }};
  pch.m_[1][1] = Patch{{
      {fx::TREMOLO, {}, {50, 50, 50, 50}}, //
      {fx::BYPASS, {}, {}},                //
      {fx::BYPASS, {}, {}},                //
  }};
  pch.m_[1][2] = Patch{{
      {fx::PHASER, {}, {50, 50, 3}}, //
      {fx::BYPASS, {}, {}},          //
      {fx::BYPASS, {}, {}},          //
  }};
  pch.m_[1][3] = Patch{{
      {fx::DELAY, {}, {50, 50, 50, 50}}, //
      {fx::BYPASS, {}, {}},              //
      {fx::BYPASS, {}, {}},              //
  }};
  pch.m_[2][0] = Patch{{
      {fx::OSCILLATOR, {false, true, false}, {50, 100, 1}}, //
      {fx::BYPASS, {}, {}},                                 //
      {fx::BYPASS, {}, {}},                                 //
  }};
  pch.m_[2][1] = Patch{{
      {fx::BOOSTER, {}, {7, 5, 5}},          //
      {fx::OVERDRIVE, {}, {75, 75, 50, 50}}, //
      {fx::BYPASS, {}, {}},                  //
  }};
  pch.m_[2][2] = Patch{{
      {fx::BOOSTER, {}, {7, 5, 5}},       //
      {fx::DISTORTION, {}, {75, 75, 50}}, //
      {fx::BYPASS, {}, {}},               //
  }};
  pch.m_[2][3] = Patch{{
      {fx::OVERDRIVE, {}, {75, 75, 50, 50}},      //
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::BYPASS, {}, {}},                       //
  }};
  pch.m_[3][0] = Patch{{
      {fx::DISTORTION, {}, {75, 75, 50}},         //
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::BYPASS, {}, {}},                       //
  }};
  pch.m_[3][1] = Patch{{
      {fx::OVERDRIVE, {}, {75, 75, 50, 50}}, //
      {fx::PHASER, {}, {50, 50, 3}},         //
      {fx::BYPASS, {}, {}},                  //
  }};
  pch.m_[3][2] = Patch{{
      {fx::TREMOLO, {}, {50, 50, 50, 50}},        //
      {fx::CHORUS, {}, {50, 50, 50, 50, 50, 50}}, //
      {fx::BYPASS, {}, {}},                       //
  }};
  pch.m_[3][3] = Patch{{
      {fx::BOOSTER, {}, {7, 5, 5}},       //
      {fx::DISTORTION, {}, {75, 75, 50}}, //
      {fx::DELAY, {}, {50, 50, 50, 50}},  //
  }};
}
