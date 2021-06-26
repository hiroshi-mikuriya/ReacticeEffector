/// @file      device/i2c.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c.h"
#include "stm32f7xx_ll_i2c.h"

namespace
{
constexpr uint32_t SIG_RXE = 1 << 0;
constexpr uint32_t SIG_TXIS = 1 << 1;
constexpr uint32_t SIG_STOP = 1 << 2;
constexpr uint32_t SIG_NACK = 1 << 3;
constexpr uint32_t SIG_ERR = 1 << 10;
} // namespace

satoh::I2C::I2C(I2C_TypeDef *const i2cx, osThreadId threadId) noexcept //
    : i2cx_(i2cx),                                                     //
      threadId_(threadId),                                             //
      rxbuf_(0),                                                       //
      txbuf_(0)                                                        //
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
  LL_I2C_EnableIT_RX(i2cx_);
  LL_I2C_EnableIT_STOP(i2cx_);
  LL_I2C_EnableIT_NACK(i2cx_);
  // TODO
}
void satoh::I2C::disableIRQ()
{
  LL_I2C_DisableIT_RX(i2cx_);
  LL_I2C_DisableIT_STOP(i2cx_);
  LL_I2C_DisableIT_NACK(i2cx_);
  // TODO
}

void satoh::I2C::notifyEvIRQ()
{
  if (LL_I2C_IsActiveFlag_RXNE(i2cx_))
  {
    *rxbuf_++ = LL_I2C_ReceiveData8(i2cx_);
    osSignalSet(threadId_, SIG_RXE);
  }
  else if (LL_I2C_IsActiveFlag_STOP(i2cx_))
  {
    LL_I2C_ClearFlag_STOP(i2cx_);
    osSignalSet(threadId_, SIG_STOP);
  }
  else if (LL_I2C_IsActiveFlag_NACK(i2cx_))
  {
    LL_I2C_ClearFlag_NACK(i2cx_);
    osSignalSet(threadId_, SIG_NACK);
  }
  else
  {
    osSignalSet(threadId_, SIG_ERR);
  }
}

void satoh::I2C::notifyErIRQ()
{
  osSignalSet(threadId_, SIG_ERR);
}

satoh::I2C::Result satoh::I2C::writeByte(uint8_t slaveAddr, uint8_t reg, uint8_t v) noexcept
{
  uint8_t buf[2] = {reg, v};
  return writeImpl(slaveAddr, buf, sizeof(buf));
}

satoh::I2C::Result satoh::I2C::readByte(uint8_t slaveAddr, uint8_t reg, uint8_t &v) noexcept
{
  return readBytes(slaveAddr, reg, &v, sizeof(v));
}

satoh::I2C::Result satoh::I2C::readBytes(uint8_t slaveAddr, uint8_t reg, uint8_t *buf, uint8_t size) noexcept
{
  auto res = writeImpl(slaveAddr, &reg, sizeof(reg));
  if (res != Result::OK)
  {
    return res;
  }
  return readImpl(slaveAddr, buf, size);
}

satoh::I2C::Result satoh::I2C::writeImpl(uint8_t slaveAddr, uint8_t const *bytes, uint8_t size) noexcept
{
  if (LL_I2C_IsActiveFlag_BUSY(i2cx_) == SET)
  {
    return Result::BUSY;
  }
  txbuf_ = bytes;
  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
  for (uint8_t i = 0; i < size; i++)
  {
    // TODO 割り込みを使ってビジーループをやめる
    while (LL_I2C_IsActiveFlag_TXIS(i2cx_) == RESET)
      osDelay(0);
    LL_I2C_TransmitData8(i2cx_, bytes[i]);
  }
  osSignalWait(SIG_STOP, osWaitForever);
  clearRegister();
  return Result::OK;
}

satoh::I2C::Result satoh::I2C::readImpl(uint8_t slaveAddr, uint8_t *buffer, uint8_t size) noexcept
{
  if (LL_I2C_IsActiveFlag_BUSY(i2cx_) == SET)
  {
    return Result::BUSY;
  }
  rxbuf_ = buffer;
  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
  for (uint8_t i = 0; i < size; i++)
  {
    // TODO signal取りこぼしするかもしれないのでストリームバッファのほうがよいと思う
    osSignalWait(SIG_RXE, osWaitForever);
  }
  osSignalWait(SIG_STOP, osWaitForever);
  clearRegister();
  return Result::OK;
}

void satoh::I2C::clearRegister() const noexcept
{
  (i2cx_->CR2 &= (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
}