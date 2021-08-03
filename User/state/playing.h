/// @file      state/playing.h
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
  /// プロパティ
  Property &m_;
  /// @brief MODE_KEYを処理する @param[in] src MODE_KEY @return 次の状態ID
  ID run(msg::MODE_KEY const *src) noexcept override;
  /// @brief EFFECT_KEYを処理する @param[in] src EFFECT_KEY @return 次の状態ID
  ID run(msg::EFFECT_KEY const *src) noexcept override;
  /// @brief ACC_GYROを処理する @param[in] src ACC_GYRO @return 次の状態ID
  ID run(msg::ACC_GYRO const *src) noexcept override;
  /// @brief ROTARY_ENCODERを処理する @param[in] src ROTARY_ENCODER @return 次の状態ID
  ID run(msg::ROTARY_ENCODER const *src) noexcept override;
  /// @brief ERRORを処理する @param[in] src ERROR @return 次の状態ID
  ID run(msg::ERROR const *src) noexcept override;
  /// @brief タイマー通知を処理する @return 次の状態ID
  ID timer() noexcept override;
  /// @brief バンク更新
  void modBank() noexcept;

public:
  /// @brief コンストラクタ
  /// @param[in] prop プロパティ
  explicit Playing(Property &prop) : m_(prop) {}
  /// @brief デストラクタ
  ~Playing() {}
  /// @brief 状態IDを取得する @return 状態ID
  ID id() const noexcept override { return PLAYING; }
  /// @brief この状態に遷移したときに行う初期化処理
  void init() noexcept override;
  /// @brief この状態が終了するときに行う終了処理
  void deinit() noexcept override;
};
