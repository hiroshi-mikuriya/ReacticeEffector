/// @file      task/i2c_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_task.h"
#include "cmsis_os.h"
#include "device/gyro.h"
#include "device/i2c.h"
#include "stm32f7xx_ll_i2c.h"
#include "usbd_cdc_if.h"

namespace
{
satoh::I2C *s_i2c = 0;
}

void i2cTaskImpl(void const *argument)
{
  extern osThreadId i2cTaskHandle;
  s_i2c = new satoh::I2C(I2C1, i2cTaskHandle);
  osDelay(10); // なんとなく
  satoh::Gyro mpu(s_i2c, satoh::MPU6050);
  for (;;)
  {
    if (mpu.ok())
    {
      int16_t acc[3] = {0};
      int16_t gyro[3] = {0};
      if (mpu.getAccelGyro(acc, gyro))
      {
        CDC_Transmit_FS(reinterpret_cast<uint8_t *>(acc), sizeof(acc));
      }
    }
    osDelay(100);
  }
}

void i2cEvIRQ()
{
  s_i2c->notifyEvIRQ();
}

void i2cErIRQ()
{
  s_i2c->notifyErIRQ();
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
