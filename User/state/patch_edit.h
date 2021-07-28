/// @file      state/patch_edit.h
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
class PatchEdit;
}
} // namespace satoh

/// @brief パッチエディット状態
class satoh::state::PatchEdit : public satoh::state::Base
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
  /// @brief ERRORを処理する
  /// @param[in] src ERROR
  /// @return 次の状態ID
  ID run(msg::ERROR const *src) noexcept override;
  /// @brief 表示更新
  void updateDisplay() noexcept;
  /// @brief 選択中のFX番号更新
  /// @param[in] up
  ///   @arg true カウントアップ
  ///   @arg false カウントダウン
  void modSelectedFxNum(bool up) noexcept;
  /// @brief 選択中のFX番号のFXを変更
  /// @param[in] next
  ///   @arg true 1つ次へ
  ///   @arg false 1つ前へ
  void modFxList(bool next) noexcept;

public:
  /// @brief コンストラクタ
  /// @param[in] prop プロパティ
  explicit PatchEdit(Property &prop) : m_(prop) {}
  /// @brief デストラクタ
  ~PatchEdit() {}
  /// @brief 状態IDを取得する @return 状態ID
  ID id() const noexcept override { return PATCH_EDIT; }
  /// @brief この状態に遷移したときに行う初期化処理
  void init() noexcept override;
  /// @brief この状態が終了するときに行う終了処理
  void deinit() noexcept override;
};
