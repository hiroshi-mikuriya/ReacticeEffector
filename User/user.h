/// @file      user.h
/// @author    Hiroshi Mikuriya
/// @copyright Copyright© 2021 Hiroshi Mikuriya
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief StartDefaultTask内部処理
    /// @param[in] argument タスク引数
    void StartDefaultTaskImpl(void const *argument);

#ifdef __cplusplus
}
#endif
