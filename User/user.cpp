/// @file      user.cpp
/// @author    Hiroshi Mikuriya
/// @copyright Copyright© 2021 Hiroshi Mikuriya
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "user.h"
#include "cmsis_os.h"
#include "main.h"

void StartDefaultTaskImpl(void const *argument)
{
    for (;;)
    {
        LL_GPIO_TogglePin(LED_ACT_GPIO_Port, LED_ACT_Pin);
        osDelay(500);
    }
}