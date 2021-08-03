/// @file      task/app_tasl.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /// Appタスクハンドル
  extern osThreadId appTaskHandle;

  /// @brief appTask内部処理
  /// @param[in] argument タスク引数
  void appTaskProc(void const *argument);

  /// @brief APP TIM割り込み
  void appTimIRQ(void);

#ifdef __cplusplus
}
#endif
