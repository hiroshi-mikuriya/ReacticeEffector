/// @file      state/common.h
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
/// @brief ACC_GYROを処理する @param[in] prop プロパティ @param[in] src ACC_GYRO
void proc(Property &prop, msg::ACC_GYRO const *src) noexcept;
//// @brief TAPボタン処理 @param[in] prop プロパティ
void tapProc(Property &prop) noexcept;
//// @brief RE1ボタン処理 @param[in] prop プロパティ
void re1Proc(Property &prop) noexcept;
//// @brief タイマー処理 @param[in] prop プロパティ
void timerProc(Property &prop) noexcept;
} // namespace state
} // namespace satoh
