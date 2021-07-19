/// @file      task/app_state/error.h
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
class Error;
}
} // namespace satoh

/// @brief 演奏状態
class satoh::state::Error : public satoh::state::Base
{
  /// プロパティ
  Property &m_;
  /// @brief MODE_KEYを処理する
  /// @param[in] src MODE_KEY
  /// @return 次の状態ID
  ID run(msg::MODE_KEY const *src) noexcept override;
  /// @brief EFFECT_KEYを処理する
  /// @param[in] src EFFECT_KEY
  /// @return 次の状態ID
  ID run(msg::EFFECT_KEY const *src) noexcept override;
  /// @brief ACC_GYROを処理する
  /// @param[in] src ACC_GYRO
  /// @return 次の状態ID
  ID run(msg::ACC_GYRO const *src) noexcept override;
  /// @brief ROTARY_ENCODERを処理する
  /// @param[in] src ROTARY_ENCODER
  /// @return 次の状態ID
  ID run(msg::ROTARY_ENCODER const *src) noexcept override;

public:
  /// @brief コンストラクタ
  /// @param[in] prop プロパティ
  explicit Error(Property &prop) : m_(prop) {}
  /// @brief デストラクタ
  ~Error() {}
  /// @brief この状態に遷移したときに行う初期化処理
  void init() noexcept override;
  /// @brief この状態が終了するときに行う終了処理
  void deinit() noexcept override;
};
