/// @file      task/i2c_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_task.h"
#include "device/gyro.h"
#include "device/level_meter.h"
#include "device/pca9635.h"
#include "message/msglib.h"
#include "stm32f7xx_ll_dma.h"
#include "usb_task.h"

namespace
{
satoh::I2C *s_i2c = 0;
} // namespace

void i2cTaskProc(void const *argument)
{
  satoh::addMsgTarget(4);
  s_i2c = new satoh::I2C(I2C1, i2cTaskHandle, DMA1, LL_DMA_STREAM_0, LL_DMA_STREAM_7);
  satoh::Gyro mpu6050(s_i2c, satoh::MPU6050);
  satoh::Gyro icm20602(s_i2c, satoh::ICM20602);
  satoh::PCA9635 led(s_i2c);
  satoh::LevelMeter level(s_i2c);
  for (;;)
  {
    auto res = satoh::recvMsg();
    if (res.status() != osOK)
    {
      continue;
    }
    auto *msg = res.msg();
    if (msg == 0)
    {
      continue;
    }
    switch (msg->type)
    {
    case satoh::msg::LED_LEVEL_UPDATE_REQ:
    {
      auto *param = reinterpret_cast<satoh::msg::LED_LEVEL const *>(msg->bytes);
      level.setLeft(param->left);
      level.setRight(param->right);
      level.show();
      break;
    }
    case satoh::msg::LED_SIMPLE_REQ:
    {
      auto *param = reinterpret_cast<satoh::msg::LED_SIMPLE const *>(msg->bytes);
      if (param->led == 0)
      {
        level.setPower(param->level);
      }
      else
      {
        level.setModulation(param->level);
      }
      level.show();
      break;
    }
    case satoh::msg::LED_EFFECT_REQ:
    {
      auto *param = reinterpret_cast<satoh::msg::LED_EFFECT const *>(msg->bytes);
      led.set(param->rgb, param->led);
      break;
    }
    case satoh::msg::GYRO_UPDATE_REQ:
    {
      if (mpu6050.ok())
      {
        satoh::msg::ACC_GYRO ag{};
        if (mpu6050.getAccelGyro(ag.acc, ag.gyro))
        {
          satoh::sendMsg(usbTxTaskHandle, satoh::msg::USB_TX_REQ, &ag, sizeof(ag));
        }
      }
      if (icm20602.ok())
      {
        satoh::msg::ACC_GYRO ag{};
        if (icm20602.getAccelGyro(ag.acc, ag.gyro))
        {
          satoh::sendMsg(usbTxTaskHandle, satoh::msg::USB_TX_REQ, &ag, sizeof(ag));
        }
      }
      break;
    }
    case satoh::msg::KEY_UPDATE_REQ:
    {
      // TODO
      break;
    }
    case satoh::msg::ENCODER_UPDATE_REQ:
    {
      // TODO
      break;
    }
    }
  }
}

void i2cEvIRQ(void)
{
  s_i2c->notifyEvIRQ();
}

void i2cErIRQ(void)
{
  s_i2c->notifyErIRQ();
}

void i2cRxEndIRQ(void)
{
  s_i2c->notifyRxEndIRQ();
}

void i2cRxErrorIRQ(void)
{
  s_i2c->notifyRxErrorIRQ();
}

void i2cTxEndIRQ(void)
{
  s_i2c->notifyTxEndIRQ();
}

void i2cTxErrorIRQ(void)
{
  s_i2c->notifyTxErrorIRQ();
}

void extiSwIRQ(void)
{
  // satoh::sendMsg(i2cTaskHandle, satoh::msg::ENCODER_UPDATE_REQ);
}

void extiSw2IRQ(void)
{
  // satoh::sendMsg(i2cTaskHandle, satoh::msg::KEY_UPDATE_REQ);
}

void extiMpuIRQ(void)
{
  // satoh::sendMsg(i2cTaskHandle, satoh::msg::GYRO_UPDATE_REQ);
}
