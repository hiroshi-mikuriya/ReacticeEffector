/// @file      task/i2c_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_task.h"
#include "cmsis_os.h"
#include "device/i2c.h"
#include "device/icm20602.h"
#include "device/mpu6050.h"
#include "stm32f7xx_ll_i2c.h"
#include "usbd_cdc_if.h"

namespace
{
satoh::I2C *s_i2c = 0;
}

void i2cTaskImpl(void const *argument)
{
  // using namespace icm20602;
  using namespace mpu6050;
  extern osThreadId i2cTaskHandle;
  s_i2c = new satoh::I2C(I2C1, i2cTaskHandle);
  s_i2c->enableIRQ();
  osDelay(10); // なんとなく
  uint8_t iam = 0;
  s_i2c->readByte(SLAVE_ADDR, WHOAMI, iam);
  s_i2c->writeByte(SLAVE_ADDR, PWR_MGMT_1, 0);
  for (;;)
  {
    uint8_t buf[6] = {0};
    s_i2c->readBytes(SLAVE_ADDR, ACCEL_XOUT_H, buf, sizeof(buf));
    CDC_Transmit_FS(buf, sizeof(buf));
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
