/// @file      device/i2c.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c.h"
#include "stm32f7xx_ll_i2c.h"

namespace
{
constexpr uint32_t SIG_TC = 1 << 0;
constexpr uint32_t SIG_TCR = 1 << 1;
constexpr uint32_t SIG_STOP = 1 << 2;
constexpr uint32_t SIG_NACK = 1 << 3;
constexpr uint32_t SIG_ERR = 1 << 10;

/// @brief I2C通信後の終了処理を行うクラス
class Finalizer
{
  /// I2Cペリフェラル
  I2C_TypeDef *const i2cx_;

  /// @brief レジスタをクリアする
  void clearRegister() const noexcept
  {
    (i2cx_->CR2 &= (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
  }
  /// @brief I2C割り込みを停止する
  void disableIT() const noexcept
  {
    LL_I2C_DisableIT_TX(i2cx_);
    LL_I2C_DisableIT_RX(i2cx_);
    LL_I2C_DisableIT_ADDR(i2cx_);
    LL_I2C_DisableIT_NACK(i2cx_);
    LL_I2C_DisableIT_STOP(i2cx_);
    LL_I2C_DisableIT_TC(i2cx_);
    LL_I2C_DisableIT_ERR(i2cx_);
  }

public:
  /// @brief コンストラクタ
  /// @param[in] i2cx I2Cペリフェラル
  explicit Finalizer(I2C_TypeDef *const i2cx) noexcept : i2cx_(i2cx) {}
  /// @brief デストラクタ
  virtual ~Finalizer()
  {
    disableIT();
    clearRegister();
    osSignalWait(0xFF, 0); // シグナルクリア
  }
};
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
  LL_I2C_Disable(i2cx_);
}

void satoh::I2C::notifyEvIRQ()
{
  if (LL_I2C_IsActiveFlag_RXNE(i2cx_))
  {
    *rxbuf_++ = LL_I2C_ReceiveData8(i2cx_);
  }
  else if (LL_I2C_IsActiveFlag_TXIS(i2cx_))
  {
    LL_I2C_TransmitData8(i2cx_, *txbuf_++);
  }
  else if (LL_I2C_IsActiveFlag_TC(i2cx_))
  {
    osSignalSet(threadId_, SIG_TC);
  }
  else if (LL_I2C_IsActiveFlag_TCR(i2cx_))
  {
    osSignalSet(threadId_, SIG_TCR);
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
  if (LL_I2C_IsActiveFlag_ARLO(i2cx_))
  {
    LL_I2C_ClearFlag_ARLO(i2cx_);
    osSignalSet(threadId_, SIG_ERR);
  }
  else if (LL_I2C_IsActiveFlag_BERR(i2cx_))
  {
    LL_I2C_ClearFlag_BERR(i2cx_);
    osSignalSet(threadId_, SIG_ERR);
  }
  else if (LL_I2C_IsActiveFlag_OVR(i2cx_))
  {
    LL_I2C_ClearFlag_OVR(i2cx_);
    osSignalSet(threadId_, SIG_ERR);
  }
  else if (LL_I2C_IsActiveSMBusFlag_TIMEOUT(i2cx_))
  {
    LL_I2C_ClearSMBusFlag_TIMEOUT(i2cx_);
    osSignalSet(threadId_, SIG_ERR);
  }
  else if (LL_I2C_IsActiveSMBusFlag_PECERR(i2cx_))
  {
    LL_I2C_ClearSMBusFlag_PECERR(i2cx_);
    osSignalSet(threadId_, SIG_ERR);
  }
  else if (LL_I2C_IsActiveSMBusFlag_ALERT(i2cx_))
  {
    LL_I2C_ClearSMBusFlag_ALERT(i2cx_);
    osSignalSet(threadId_, SIG_ERR);
  }
  else
  {
    osSignalSet(threadId_, SIG_ERR);
  }
}

/// @brief シグナルを待機し、エラーが発生したらリターンする
/// @param[in] sigs 待機するシグナルの種類
/// @param[in] timeout タイムアウト時間（ミリ秒）
#define WAIT_SIGNAL(sigs, timeout)                \
  do                                              \
  {                                               \
    osEvent ev = osSignalWait((sigs), (timeout)); \
    if (ev.status == osEventTimeout)              \
    {                                             \
      LL_I2C_Disable(i2cx_);                      \
      LL_I2C_Enable(i2cx_);                       \
      return Result::TIMEOUT;                     \
    }                                             \
    if (ev.value.signals & SIG_NACK)              \
    {                                             \
      LL_I2C_Disable(i2cx_);                      \
      LL_I2C_Enable(i2cx_);                       \
      return Result::NACK;                        \
    }                                             \
  } while (0)

satoh::I2C::Result satoh::I2C::write(uint8_t slaveAddr, uint8_t const *bytes, uint32_t size) noexcept
{
  if (LL_I2C_IsActiveFlag_BUSY(i2cx_))
  {
    return Result::BUSY;
  }
  Finalizer fin(i2cx_);
  txbuf_ = bytes;
  LL_I2C_EnableIT_STOP(i2cx_);
  // LL_I2C_EnableIT_TX(i2cx_);
  // LL_I2C_EnableIT_TC(i2cx_);
  LL_I2C_EnableIT_NACK(i2cx_);
  LL_I2C_EnableIT_ERR(i2cx_);
  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
  // WAIT_SIGNAL(SIG_TC | SIG_NACK, 10);
  for (uint32_t i = 0; i < size; ++i)
  {
    // TODO 送信完了割り込みを待つようにしたい。
    // MPU6050ではうまくいったが、PCM9635ではうまくいかなかったので暫定的にビジーループに戻す。
    while (!LL_I2C_IsActiveFlag_TXIS(i2cx_))
    {
      auto ev = osSignalWait(SIG_NACK, 0);
      if (ev.value.signals & SIG_NACK)
      {
        LL_I2C_Disable(i2cx_);
        LL_I2C_Enable(i2cx_);
        return Result::NACK;
      }
    }
    LL_I2C_TransmitData8(i2cx_, *txbuf_++);
  }
  WAIT_SIGNAL(SIG_STOP | SIG_NACK, 2);
  return Result::OK;
}

satoh::I2C::Result satoh::I2C::read(uint8_t slaveAddr, uint8_t *buffer, uint32_t size) noexcept
{
  if (LL_I2C_IsActiveFlag_BUSY(i2cx_))
  {
    return Result::BUSY;
  }
  Finalizer fin(i2cx_);
  rxbuf_ = buffer;
  LL_I2C_EnableIT_RX(i2cx_);
  LL_I2C_EnableIT_STOP(i2cx_);
  LL_I2C_EnableIT_NACK(i2cx_);
  LL_I2C_EnableIT_ERR(i2cx_);
  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
  WAIT_SIGNAL(SIG_STOP | SIG_NACK, 10);
  return Result::OK;
}
