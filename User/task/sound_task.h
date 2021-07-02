/// @file      task/sound_task.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"
#include "stm32f7xx.h"

#ifdef __cplusplus
#include "device/i2c.h"
/// @brief PCM3060初期化
/// @param[in] i2c I2C通信クラス
/// @note I2Cタスクから呼ぶこと
void initPCM3060(satoh::I2C *i2c);
extern "C"
{
#endif

  /// Soundタスクハンドル
  extern osThreadId soundTaskHandle;

  /// @brief soundTask内部処理
  /// @param[in] argument タスク引数
  void soundTaskProc(void const *argument);

#ifdef __cplusplus
}
#endif
