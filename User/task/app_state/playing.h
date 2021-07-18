/// @file      task/app_state/playing.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "state_base.h"

namespace satoh
{
namespace state
{
class Playing;
}
} // namespace satoh

/// @brief 演奏状態
class satoh::state::Playing : public satoh::state::Base
{
  /// @brief MODE_KEYを処理する
  /// @param[in] src MODE_KEY
  /// @param[in] prop プロパティ
  /// @return 次の状態ID
  ID run(msg::MODE_KEY const *src, Property &prop) noexcept override;
  /// @brief EFFECT_KEYを処理する
  /// @param[in] src EFFECT_KEY
  /// @param[in] prop プロパティ
  /// @return 次の状態ID
  ID run(msg::EFFECT_KEY const *src, Property &prop) noexcept override;
  /// @brief ACC_GYROを処理する
  /// @param[in] src ACC_GYRO
  /// @param[in] prop プロパティ
  /// @return 次の状態ID
  ID run(msg::ACC_GYRO const *src, Property &prop) noexcept override;
  /// @brief ROTARY_ENCODERを処理する
  /// @param[in] src ROTARY_ENCODER
  /// @param[in] prop プロパティ
  /// @return 次の状態ID
  ID run(msg::ROTARY_ENCODER const *src, Property &prop) noexcept override;
  /// @brief バンク更新
  /// @param[in] prop プロパティ
  void modBank(Property &prop) noexcept;

public:
  /// @brief デストラクタ
  ~Playing() {}
  /// @brief この状態に遷移したときに行う初期化処理
  /// @param[in] prop プロパティ
  void init(Property &prop) noexcept override;
  /// @brief この状態が終了するときに行う終了処理
  /// @param[in] prop プロパティ
  void deinit(Property &prop) noexcept override;
};
