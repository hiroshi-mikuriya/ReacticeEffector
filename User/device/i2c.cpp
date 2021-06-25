/// @file      device/i2c.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c.h"
#include "stm32f7xx_ll_i2c.h"

void satoh::I2C::writeImpl(uint8_t slaveAddr, uint8_t const *bytes, uint8_t size) const noexcept
{
  // busyフラグをチェック
  while (LL_I2C_IsActiveFlag_BUSY(i2cx_) == SET)
    ;

  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

  for (uint8_t i = 0; i < size; i++)
  {
    while (LL_I2C_IsActiveFlag_TXE(i2cx_) == RESET)
      ;
    LL_I2C_TransmitData8(i2cx_, bytes[i]);
  }

  while (LL_I2C_IsActiveFlag_STOP(i2cx_) == RESET)
    ;
  LL_I2C_ClearFlag_STOP(i2cx_);

  // 設定をリセットする
  (i2cx_->CR2 &= (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
}

void satoh::I2C::readImpl(uint8_t slaveAddr, uint8_t *buffer, uint8_t size) const noexcept
{
  // busyフラグをチェック
  while (LL_I2C_IsActiveFlag_BUSY(i2cx_) == SET)
    ;

  // 初期設定
  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

  for (uint8_t i = 0; i < size; i++)
  {
    while (LL_I2C_IsActiveFlag_RXNE(i2cx_) == RESET)
      ;
    buffer[i] = LL_I2C_ReceiveData8(i2cx_);
  }

  while (LL_I2C_IsActiveFlag_STOP(i2cx_) == RESET)
    ;
  LL_I2C_ClearFlag_STOP(i2cx_);

  // 設定をリセットする
  (i2cx_->CR2 &= (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
}

satoh::I2C::I2C(I2C_TypeDef *const i2cx, osThreadId task) noexcept : i2cx_(i2cx), task_(task)
{
  LL_I2C_Enable(i2cx_);
}

satoh::I2C::~I2C()
{
  disableIRQ();
  LL_I2C_Disable(i2cx_);
}

void satoh::I2C::enableIRQ()
{
  // TODO
}
void satoh::I2C::disableIRQ()
{
  // TODO
}

void satoh::I2C::notifyEvIRQ()
{
  // TODO
}

void satoh::I2C::notifyErIRQ()
{
  // TODO
}

void satoh::I2C::writeByte(uint8_t slaveAddr, uint8_t reg, uint8_t v) noexcept
{
  uint8_t buf[2] = {reg, v};
  writeImpl(slaveAddr, buf, sizeof(buf));
}

uint8_t satoh::I2C::readByte(uint8_t slaveAddr, uint8_t reg) noexcept
{
  uint8_t v = 0;
  readBytes(slaveAddr, reg, &v, sizeof(v));
  return v;
}

void satoh::I2C::readBytes(uint8_t slaveAddr, uint8_t reg, uint8_t *buf, uint8_t size) noexcept
{
  writeImpl(slaveAddr, &reg, sizeof(reg));
  readImpl(slaveAddr, buf, size);
}
