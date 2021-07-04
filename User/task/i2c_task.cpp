/// @file      task/i2c_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c_task.h"
#include "device/gyro.h"
#include "device/level_meter.h"
#include "device/pca9635.h"
#include "device/rotary_encoder.h"
#include "device/ssd1306.h"
#include "message/msglib.h"
#include "stm32f7xx_ll_dma.h"
#include "task/app_task.h"
#include "task/sound_task.h"
#include "task/usb_task.h"

namespace
{
satoh::I2C *s_i2c = 0; ///< I2C通信オブジェクト

/// @brief ジャイロ値取得処理
/// @param[in] mpu6050 MPU6050と通信するオブジェクト
/// @param[in] icm20602 ICM20602と通信するオブジェクト
void gyroGetProc(satoh::Gyro const &mpu6050, satoh::Gyro const &icm20602) noexcept
{
  if (mpu6050.ok())
  {
    satoh::msg::ACC_GYRO ag{};
    if (mpu6050.getAccelGyro(ag.acc, ag.gyro))
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::GYRO_NOTIFY, &ag, sizeof(ag));
    }
    return;
  }
  if (icm20602.ok())
  {
    satoh::msg::ACC_GYRO ag{};
    if (icm20602.getAccelGyro(ag.acc, ag.gyro))
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::GYRO_NOTIFY, &ag, sizeof(ag));
    }
    return;
  }
}
/// @brief レベルメーター更新処理
/// @param[in] level レベルメーターと通信するオブジェクト
/// @param[in] msg リクエストメッセージ
void ledLevelUpdateProc(satoh::LevelMeter &level, satoh::Message const *msg) noexcept
{
  if (level.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::LED_LEVEL const *>(msg->bytes);
    level.setLeft(param->left);
    level.setRight(param->right);
    level.show();
  }
}
/// @brief Power/Modulation LED点灯・消灯処理
/// @param[in] level レベルメーターと通信するオブジェクト
/// @param[in] msg リクエストメッセージ
void ledSimpleProc(satoh::LevelMeter &level, satoh::Message const *msg) noexcept
{
  if (level.ok())
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
  }
}
/// @brief エフェクトLED更新処理
/// @param[in] led エフェクトLEDと通信するオブジェクト
/// @param[in] msg リクエストメッセージ
void ledEffectProc(satoh::PCA9635 &led, satoh::Message const *msg) noexcept
{
  if (led.ok())
  {
    auto *param = reinterpret_cast<satoh::msg::LED_EFFECT const *>(msg->bytes);
    led.set(param->rgb, param->led);
  }
}
/// @brief キー状態取得処理
void keyUpdateProc()
{
  // TODO
}
/// @brief エンコーダ状態取得処理
void encoderGetProc(satoh::RotaryEncoder &encoder)
{
  if (encoder.ok())
  {
    satoh::msg::ROTARY_ENCODER enc{};
    satoh::msg::EFFECT_KEY key{};
    int res = encoder.read(key.button, enc.angleDiff);
    if (res & 1)
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::ROTARY_ENCODER_NOTIFY, &enc, sizeof(enc));
    }
    if (res & 2)
    {
      satoh::sendMsg(appTaskHandle, satoh::msg::EFFECT_KEY_CHANGED_NOTIFY, &key, sizeof(key));
    }
  }
}
/// @brief OLED表示更新処理
/// @param[in] oled OLED通信オブジェクト
/// @param[in] msg リクエストメッセージ
void oledGetProc(satoh::SSD1306 &oled, satoh::Message const *msg)
{
  if (oled.ok())
  {
    // TODO
  }
}
} // namespace

void i2cTaskProc(void const *argument)
{
  satoh::addMsgTarget(4);
  s_i2c = new satoh::I2C(I2C1, i2cTaskHandle, DMA1, LL_DMA_STREAM_0, LL_DMA_STREAM_7);
  initPCM3060(s_i2c);
  satoh::SSD1306 oled(s_i2c);
  satoh::PCA9635 led(s_i2c);
  satoh::LevelMeter level(s_i2c);
  satoh::RotaryEncoder encoder(s_i2c);
  satoh::Gyro mpu6050(s_i2c, satoh::MPU6050);
  satoh::Gyro icm20602(s_i2c, satoh::ICM20602);
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
      ledLevelUpdateProc(level, msg);
      break;
    case satoh::msg::LED_SIMPLE_REQ:
      ledSimpleProc(level, msg);
      break;
    case satoh::msg::LED_EFFECT_REQ:
      ledEffectProc(led, msg);
      break;
    case satoh::msg::GYRO_GET_REQ:
      gyroGetProc(mpu6050, icm20602);
      break;
    case satoh::msg::MODE_KEY_GET_REQ:
      keyUpdateProc(/* key */);
      break;
    case satoh::msg::ENCODER_GET_REQ:
      encoderGetProc(encoder);
      break;
    case satoh::msg::OLED_UPDATE_REQ:
      oledGetProc(oled, msg);
      break;
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
  satoh::sendMsg(i2cTaskHandle, satoh::msg::ENCODER_GET_REQ);
}

void extiSw2IRQ(void)
{
  satoh::sendMsg(i2cTaskHandle, satoh::msg::MODE_KEY_GET_REQ);
}

void extiMpuIRQ(void)
{
  satoh::sendMsg(i2cTaskHandle, satoh::msg::GYRO_GET_REQ);
}
