/// @file      task/app_state/state_base.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "message/msglib.h"
#include "property.h"

namespace satoh
{
namespace state
{
class Base;
/// @brief 状態ID
enum ID
{
  PLAYING = 0, ///< 演奏状態
  PATCH_EDIT,  ///< パッチ編集状態
  EFFECT_EDIT, ///< エフェクトパラメータ編集状態
  ERROR,       ///< エラー状態
};
} // namespace state
} // namespace satoh

/// @brief 状態基底クラス
class satoh::state::Base
{
  /// @brief MODE_KEYを処理する
  /// @param[in] src MODE_KEY
  /// @return 次の状態ID
  virtual ID run(msg::MODE_KEY const *src) noexcept = 0;
  /// @brief EFFECT_KEYを処理する
  /// @param[in] src EFFECT_KEY
  /// @return 次の状態ID
  virtual ID run(msg::EFFECT_KEY const *src) noexcept = 0;
  /// @brief ACC_GYROを処理する
  /// @param[in] src ACC_GYRO
  /// @return 次の状態ID
  virtual ID run(msg::ACC_GYRO const *src) noexcept = 0;
  /// @brief ROTARY_ENCODERを処理する
  /// @param[in] src ROTARY_ENCODER
  /// @return 次の状態ID
  virtual ID run(msg::ROTARY_ENCODER const *src) noexcept = 0;

public:
  /// @brief デストラクタ
  virtual ~Base() {}
  /// @brief この状態に遷移したときに行う初期化処理
  virtual void init() noexcept = 0;
  /// @brief この状態が終了するときに行う終了処理
  virtual void deinit() noexcept = 0;
  /// @brief メッセージ処理
  /// @param[in] msg メッセージ
  /// @return 次の状態ID
  ID run(Message const *msg) noexcept
  {
    switch (msg->type)
    {
    case msg::MODE_KEY_NOTIFY:
      return run(reinterpret_cast<msg::MODE_KEY const *>(msg->bytes));
    case msg::EFFECT_KEY_CHANGED_NOTIFY:
      return run(reinterpret_cast<msg::EFFECT_KEY const *>(msg->bytes));
    case msg::GYRO_NOTIFY:
      return run(reinterpret_cast<msg::ACC_GYRO const *>(msg->bytes));
    case msg::ROTARY_ENCODER_NOTIFY:
      return run(reinterpret_cast<msg::ROTARY_ENCODER const *>(msg->bytes));
    }
    return ERROR;
  }
};
