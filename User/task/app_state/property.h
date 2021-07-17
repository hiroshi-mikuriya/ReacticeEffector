/// @file      task/app_state/property.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/utils.h"
#include "constant.h"
#include "effector/booster.hpp"
#include "effector/bq_filter.hpp"
#include "effector/bypass.hpp"
#include "effector/chorus.hpp"
#include "effector/delay.hpp"
#include "effector/distortion.hpp"
#include "effector/oscillator.hpp"
#include "effector/overdrive.hpp"
#include "effector/phaser.hpp"
#include "effector/reverb.hpp"
#include "effector/tremolo.hpp"

namespace satoh
{
namespace state
{
struct Effectors;
struct EffectParam;
struct Patch;
struct Property;
} // namespace state
} // namespace satoh

/// @brief エフェクター一覧型
class satoh::state::Effectors
{
  Bypass bypass_;
  Booster booster_;
  OverDrive overDrive_;
  Distortion distortion_;
  Chorus chorus_;
  Phaser phaser_;
  Tremolo tremolo_;
  // Delay delay_;
  // Reverb reverb_;
  Oscillator oscillator_;
  BqFilter bqFilter_;

public:
  /// 使用可能なエフェクター一覧
  EffectorBase *list[9];
  /// @brief コンストラクタ
  Effectors()
  {
    auto **p = list;
    *(p++) = &bypass_;
    *(p++) = &booster_;
    *(p++) = &overDrive_;
    *(p++) = &distortion_;
    *(p++) = &chorus_;
    *(p++) = &phaser_;
    *(p++) = &tremolo_;
    *(p++) = &oscillator_;
    *(p++) = &bqFilter_;
  }
};

/// @brief １つのエフェクターが持つデータ
struct satoh::state::EffectParam
{
  fx::ID id;
  bool gyro[6];
  float value[6];
};

/// @brief １つのパッチが持つデータ
struct satoh::state::Patch
{
  EffectorBase *fx[MAX_EFFECTOR_COUNT];
  EffectParam param[MAX_EFFECTOR_COUNT];
};

/// @brief 状態プロパティ
struct satoh::state::Property
{
  /// バンク番号
  uint8_t bankNum = 0;
  /// パッチ番号
  uint8_t patchNum = 0;
  /// 編集中のエフェクト番号
  uint8_t editSelectedFxNum = 0;
  /// エフェクター実態
  Effectors effectors[MAX_EFFECTOR_COUNT];
  /// パッチデータ
  Patch patches[MAX_BANK][MAX_PATCH];
  /// @brief コンストラクタ
  Property()
  {
    for (size_t pix = 0; pix < MAX_PATCH; ++pix)
    {
      for (size_t bix = 0; bix < MAX_BANK; ++bix)
      {
        auto &pch = patches[bix][pix];
        for (size_t i = 0; i < satoh::countof(pch.fx); ++i)
        {
          pch.fx[i] = effectors[i].list[0];
        }
        patches[bix][pix] = std::move(pch);
      }
    }
  }
  /// @brief 選択中のパッチを取得
  /// @return 選択中のパッチ
  Patch &getCurrectPatch() noexcept { return patches[bankNum][patchNum]; }
};
