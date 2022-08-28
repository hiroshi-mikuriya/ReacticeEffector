/// @file      task/handles.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"

extern osThreadId usbTxTaskHandle;
extern osThreadId i2cTaskHandle;
extern osThreadId neoPixelTaskHandle;
extern osThreadId appTaskHandle;
extern osThreadId soundTaskHandle;
extern osThreadId adcTaskHandle;
extern osThreadId i2cMonitorTaskHandle;
