/// @file      task/i2c_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_task.h"
#include "cmsis_os.h"
#include "device/gyro.h"
#include "device/i2c.h"
#include "device/pca9635.h"
#include "stm32f7xx_ll_i2c.h"
#include "usbd_cdc_if.h"

namespace
{
satoh::I2C *s_i2c = 0;
}

void i2cTaskImpl(void const *argument)
{
  extern osThreadId i2cTaskHandle;
  s_i2c = new satoh::I2C(I2C1, i2cTaskHandle, DMA1, LL_DMA_STREAM_7);
  osDelay(10); // なんとなく
  satoh::Gyro mpu(s_i2c, satoh::MPU6050);
  satoh::Gyro icm(s_i2c, satoh::ICM20602);
  satoh::PCA9635 pca9635(s_i2c);
  for (int i = 0;; i = (i + 1) % 4)
  {
    pca9635.set({0x80, 0x00, 0x00}, (i + 0) % 4);
    pca9635.set({0x00, 0x80, 0x00}, (i + 1) % 4);
    pca9635.set({0x00, 0x00, 0x80}, (i + 2) % 4);
    pca9635.set({0x80, 0x80, 0x00}, (i + 3) % 4);
    if (mpu.ok())
    {
      int16_t acc[3] = {0};
      int16_t gyro[3] = {0};
      if (mpu.getAccelGyro(acc, gyro))
      {
        CDC_Transmit_FS(reinterpret_cast<uint8_t *>(acc), sizeof(acc));
      }
    }
    if (icm.ok())
    {
      int16_t acc[3] = {0};
      int16_t gyro[3] = {0};
      if (icm.getAccelGyro(acc, gyro))
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

void i2cTxEndIRQ()
{
  s_i2c->notifyTxEndIRQ();
}

void i2cTxErrorIRQ()
{
  s_i2c->notifyTxErrorIRQ();
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
