/// @file      task/i2c_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_task.h"
#include "cmsis_os.h"

void i2cTaskImpl(void const *argument)
{
  for (;;)
  {
    osDelay(1);
  }
}

void i2cEvIRQ(I2C_TypeDef *i2c)
{
  extern osThreadId i2cTaskHandle;
  // TODO
}

void i2cErIRQ(I2C_TypeDef *i2c)
{
  extern osThreadId i2cTaskHandle;
  // TODO
}

void extiSwIRQ(void)
{
  // TODO
}

void extiSw2IRQ(void)
{
  // TODO
}

void extiMpuIRQ(void)
{
  // TODO
}
