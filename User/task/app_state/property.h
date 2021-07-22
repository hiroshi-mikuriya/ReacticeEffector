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
// #include "effector/reverb.hpp"
#include "effector/tremolo.hpp"

namespace satoh
{
namespace state
{
class Effectors;
struct EffectParam;
struct Patch;
struct Property;
} // namespace state
} // namespace satoh

/// @brief エフェクター一覧型
class satoh::state::Effectors
{
  /// @brief デフォルトコンストラクタ削除
  Effectors() = delete;

  fx::Bypass bypass_;
  fx::Booster booster_;
  fx::OverDrive overDrive_;
  fx::Distortion distortion_;
  fx::Chorus chorus_;
  fx::Phaser phaser_;
  fx::Tremolo tremolo_;
  fx::Delay delay_;
  // fx::Reverb reverb_;
  fx::Oscillator oscillator_;
  fx::BqFilter bqFilter_;
  /// エフェクター一覧
  fx::EffectorBase *list_[10];

public:
  /// @brief コンストラクタ
  /// @param[in] n FX番号（0, 1, 2）
  explicit Effectors(uint8_t n)
  {
    // TODO n はDelayで使う気がするのでとっておく
    auto **p = list_;
    *(p++) = &bypass_;
    *(p++) = &booster_;
    *(p++) = &overDrive_;
    *(p++) = &distortion_;
    *(p++) = &chorus_;
    *(p++) = &phaser_;
    *(p++) = &tremolo_;
    *(p++) = &delay_;
    *(p++) = &oscillator_;
    *(p++) = &bqFilter_;
  }
  /// @brief エフェクター取得
  /// @param[in] i インデックス
  /// @return エフェクター
  fx::EffectorBase *getFx(size_t i) noexcept { return list_[i]; }
  /// @brief エフェクター取得
  /// @param[in] i インデックス
  /// @return エフェクター
  fx::EffectorBase const *getFx(size_t i) const noexcept { return list_[i]; }
  /// @brief エフェクター数を取得
  /// @return エフェクター数
  size_t count() const noexcept { return satoh::countof(list_); }
  /// @brief エフェクター一覧から検索してインデックスを返す
  /// @param[in] fx 検索対象のエフェクター
  /// @retval 0以上 インデックス
  /// @retval -1 見つからない
  /// @retval -2 引数がNULL
  int find(fx::EffectorBase const *fx)
  {
    if (fx == 0)
    {
      return -2;
    }
    for (size_t i = 0; i < count(); ++i)
    {
      if (getFx(i) == fx)
      {
        return static_cast<int>(i);
      }
    }
    return -1;
  }
  /// @brief 次のエフェクターを取得
  /// @param[in] cur 現在選択中のエフェクター
  /// @param[in] next
  ///   @arg true 次のエフェクターを検索
  ///   @arg false 前のエフェクターを検索
  /// @return 次 or 前のエフェクター
  fx::EffectorBase *getNext(fx::EffectorBase const *cur, bool next) noexcept
  {
    int ix = find(cur);
    if (ix < 0)
    {
      return getFx(0);
    }
    int d = next ? 1 : -1;
    ix = (ix + d + count()) % count();
    return getFx(ix);
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
  /// エフェクター
  fx::EffectorBase *fx[MAX_EFFECTOR_COUNT];
  /// パラメータ
  EffectParam param[MAX_EFFECTOR_COUNT];
  /// @brief fx -> param
  void dump() noexcept
  {
    for (size_t i = 0; i < satoh::countof(fx); ++i)
    {
      auto const *fx0 = fx[i];
      auto &param0 = param[i];
      param0.id = fx0->getID();
      for (uint8_t n = 0; n < fx0->getParamCount(); ++n)
      {
        param0.gyro[n] = fx0->isGyroEnabled(n);
        param0.value[n] = fx0->getParam(n);
      }
    }
  }
  /// @brief param -> fx
  void load() noexcept
  {
    for (size_t i = 0; i < satoh::countof(fx); ++i)
    {
      auto *fx0 = fx[i];
      auto const &param0 = param[i];
      if (param0.id != fx0->getID())
      {
        continue;
      }
      for (uint8_t n = 0; n < fx0->getParamCount(); ++n)
      {
        fx0->setGyroEnable(n, param0.gyro[n]);
        fx0->setParam(n, param0.value[n]);
      }
    }
  }
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
  /// 最後にタップを押した時間
  uint32_t lastTapTick = 0;
  /// @brief コンストラクタ
  Property() : effectors({Effectors(0), Effectors(1), Effectors(2)})
  {
    for (size_t bix = 0; bix < MAX_BANK; ++bix)
    {
      for (size_t pix = 0; pix < MAX_PATCH; ++pix)
      {
        auto &pch = patches[bix][pix];
        for (size_t i = 0; i < satoh::countof(pch.fx); ++i)
        {
          pch.fx[i] = effectors[i].getFx(0);
        }
        pch.dump();
      }
    }
  }
  /// @brief 選択中のパッチを取得
  /// @return 選択中のパッチ
  Patch &getCurrectPatch() noexcept { return patches[bankNum][patchNum]; }
};
