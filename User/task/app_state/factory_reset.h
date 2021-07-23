/// @file      task/app_state/factory_reset.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "property.h"

namespace satoh
{
namespace state
{
/// @brief ファクトリーリセット
/// @param[out] pch パッチ格納先
void factoryReset(PatchTable &pch) noexcept;
} // namespace state
} // namespace satoh
