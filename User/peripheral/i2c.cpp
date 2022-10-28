/// @file      peripheral/i2c.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "i2c.h"
#include <cstring> // memcpy

namespace
{
constexpr int32_t SIG_STOP = (1 << 0) & satoh::I2C_CLS_SIG_MASK;
constexpr int32_t SIG_NACK = (1 << 1) & satoh::I2C_CLS_SIG_MASK;
constexpr int32_t SIG_DMAEND = (1 << 2) & satoh::I2C_CLS_SIG_MASK;
constexpr int32_t SIG_DMAERR = (1 << 3) & satoh::I2C_CLS_SIG_MASK;
constexpr int32_t SIG_ERR = (1 << 7) & satoh::I2C_CLS_SIG_MASK;

constexpr size_t RX_BUF_SIZE = 32;
constexpr size_t TX_BUF_SIZE = 1025; // OLEDの送信バッファを想定したサイズ

/// @brief I2C通信後の終了処理を行うクラス
class Finalizer
{
  I2C_TypeDef *const i2cx_; ///< I2Cペリフェラル

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

  /// @brief シグナルクリア
  void clearSignals() const noexcept { osSignalWait(0xFF, 0); }

public:
  /// @brief コンストラクタ
  /// @param [in] i2cx I2Cペリフェラル
  explicit Finalizer(I2C_TypeDef *const i2cx) noexcept : i2cx_(i2cx) { clearSignals(); }
  /// @brief デストラクタ
  virtual ~Finalizer()
  {
    disableIT();
    clearRegister();
    clearSignals();
  }
};
/// @brief I2Cビジーが解除されるまで待機する
/// @param [in] i2cx I2Cペリフェラル
/// @retval true レディ
/// @retval false タイムアウト
inline bool waitForReady(I2C_TypeDef *i2cx)
{
  for (int i = 0; i < 100; ++i)
  {
    if (!LL_I2C_IsActiveFlag_BUSY(i2cx))
    {
      return true;
    }
    osThreadYield();
  }
  return false;
}
} // namespace

void satoh::I2C::start() const noexcept
{
  LL_I2C_Enable(i2cx_);
  LL_I2C_EnableDMAReq_RX(i2cx_);
  LL_I2C_EnableDMAReq_TX(i2cx_);
  LL_DMA_EnableIT_TC(dma_, rxStream_);
  LL_DMA_EnableIT_TE(dma_, rxStream_);
  LL_DMA_EnableIT_TC(dma_, txStream_);
  LL_DMA_EnableIT_TE(dma_, txStream_);
  LL_DMA_ConfigAddresses(dma_, txStream_,                                            //
                         reinterpret_cast<uint32_t>(txbuf_.get()),                   //
                         LL_I2C_DMA_GetRegAddr(i2cx_, LL_I2C_DMA_REG_DATA_TRANSMIT), //
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH                           //
  );
  LL_DMA_ConfigAddresses(dma_, rxStream_,                                           //
                         LL_I2C_DMA_GetRegAddr(i2cx_, LL_I2C_DMA_REG_DATA_RECEIVE), //
                         reinterpret_cast<uint32_t>(rxbuf_.get()),                  //
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY                          //
  );
}

void satoh::I2C::stop() const noexcept
{
  if (i2cx_)
  {
    LL_DMA_DisableIT_TC(dma_, rxStream_);
    LL_DMA_DisableIT_TE(dma_, rxStream_);
    LL_DMA_DisableIT_TC(dma_, txStream_);
    LL_DMA_DisableIT_TE(dma_, txStream_);
    LL_DMA_DisableStream(dma_, txStream_);
    LL_I2C_DisableDMAReq_RX(i2cx_);
    LL_I2C_DisableDMAReq_TX(i2cx_);
    LL_I2C_Disable(i2cx_);
  }
}

void satoh::I2C::restart() const noexcept
{
  stop();
  start();
}

void satoh::I2C::deinit() noexcept
{
  i2cx_ = 0;
  threadId_ = 0;
  mutex_.remove();
  dma_ = 0;
  rxStream_ = 0;
  txStream_ = 0;
  rxbuf_.reset();
  txbuf_.reset();
}

satoh::I2C::I2C()   //
    : i2cx_(0),     //
      threadId_(0), //
      dma_(0),      //
      rxStream_(0), //
      txStream_(0)  //
{
}

satoh::I2C::I2C(I2C_TypeDef *const i2cx,        //
                DMA_TypeDef *const dma,         //
                uint32_t rxStream,              //
                uint32_t txStream) noexcept     //
    : i2cx_(i2cx),                              //
      threadId_(0),                             //
      dma_(dma),                                //
      rxStream_(rxStream),                      //
      txStream_(txStream),                      //
      rxbuf_(makeDmaMem<uint8_t>(RX_BUF_SIZE)), //
      txbuf_(makeDmaMem<uint8_t>(TX_BUF_SIZE))  //
{
  start();
}

satoh::I2C::I2C(I2C &&that) noexcept
    : i2cx_(that.i2cx_),              //
      threadId_(that.threadId_),      //
      mutex_(std::move(that.mutex_)), //
      dma_(that.dma_),                //
      rxStream_(that.rxStream_),      //
      txStream_(that.txStream_),      //
      rxbuf_(std::move(that.rxbuf_)), //
      txbuf_(std::move(that.txbuf_))  //
{
  that.deinit();
  start();
}

satoh::I2C &satoh::I2C::operator=(satoh::I2C &&that) noexcept
{
  if (this != &that)
  {
    stop();
    i2cx_ = that.i2cx_;
    threadId_ = that.threadId_;
    mutex_ = std::move(that.mutex_);
    dma_ = that.dma_;
    rxStream_ = that.rxStream_;
    txStream_ = that.txStream_;
    rxbuf_ = std::move(that.rxbuf_);
    txbuf_ = std::move(that.txbuf_);
    that.deinit();
  }
  return *this;
}

satoh::I2C::~I2C()
{
  stop();
}

void satoh::I2C::notifyEvIRQ() noexcept
{
  if (LL_I2C_IsActiveFlag_STOP(i2cx_))
  {
    LL_I2C_ClearFlag_STOP(i2cx_);
    osSignalSet(threadId_, SIG_STOP);
  }
  if (LL_I2C_IsActiveFlag_NACK(i2cx_))
  {
    LL_I2C_ClearFlag_NACK(i2cx_);
    osSignalSet(threadId_, SIG_NACK);
  }
}

void satoh::I2C::notifyErIRQ() noexcept
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

void satoh::I2C::notifyRxEndIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMAEND);
}

void satoh::I2C::notifyRxErrorIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMAERR);
}

void satoh::I2C::notifyTxEndIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMAEND);
}

void satoh::I2C::notifyTxErrorIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMAERR);
}

/// @brief シグナルを待機し、エラーが発生したらリターンする
/// @param [in] sigs 待機するシグナルの種類
/// @param [in] timeout タイムアウト時間（ミリ秒）
#define WAIT_SIGNAL(sigs, timeout)                 \
  do                                               \
  {                                                \
    osEvent ev = osSignalWait((sigs), (timeout));  \
    if (ev.status == osEventTimeout)               \
    {                                              \
      restart();                                   \
      return Result::TIMEOUT;                      \
    }                                              \
    if (ev.value.signals & SIG_NACK)               \
    {                                              \
      restart();                                   \
      return Result::NACK;                         \
    }                                              \
    if (ev.value.signals & (SIG_DMAERR | SIG_ERR)) \
    {                                              \
      restart();                                   \
      return Result::ERROR;                        \
    }                                              \
  } while (0)

satoh::I2C::Result satoh::I2C::write(uint8_t slaveAddr, void const *bytes, uint32_t size, bool withSleep) const noexcept
{
  LockGuard<Mutex> lock(mutex_);
  if (!i2cx_)
  {
    return Result::ERROR;
  }
  threadId_ = osThreadGetId();
  if (threadId_ == 0)
  {
    return Result::ERROR;
  }
  if (!waitForReady(i2cx_))
  {
    restart();
    return Result::BUSY;
  }
  Finalizer fin(i2cx_);
  LL_I2C_EnableIT_NACK(i2cx_);
  LL_I2C_EnableIT_STOP(i2cx_);
  LL_I2C_EnableIT_ERR(i2cx_);
  memcpy(txbuf_.get(), bytes, size);
  LL_DMA_SetDataLength(dma_, txStream_, size);
  LL_DMA_EnableStream(dma_, txStream_);
  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
  WAIT_SIGNAL(SIG_DMAEND | SIG_DMAERR | SIG_NACK | SIG_ERR, 100);
  WAIT_SIGNAL(SIG_STOP, 1);
  if (withSleep)
  {
    osDelay(1);
  }
  return Result::OK;
}

satoh::I2C::Result satoh::I2C::read(uint8_t slaveAddr, void *buffer, uint32_t size, bool withSleep) const noexcept
{
  LockGuard<Mutex> lock(mutex_);
  if (!i2cx_)
  {
    return Result::ERROR;
  }
  threadId_ = osThreadGetId();
  if (threadId_ == 0)
  {
    return Result::ERROR;
  }
  if (!waitForReady(i2cx_))
  {
    restart();
    return Result::BUSY;
  }
  Finalizer fin(i2cx_);
  LL_I2C_EnableIT_STOP(i2cx_);
  LL_I2C_EnableIT_NACK(i2cx_);
  LL_I2C_EnableIT_ERR(i2cx_);
  LL_DMA_SetDataLength(dma_, rxStream_, size);
  LL_DMA_EnableStream(dma_, rxStream_);
  LL_I2C_HandleTransfer(i2cx_, slaveAddr, LL_I2C_ADDRSLAVE_7BIT, size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
  WAIT_SIGNAL(SIG_DMAEND | SIG_DMAERR | SIG_NACK | SIG_ERR, 100);
  WAIT_SIGNAL(SIG_STOP, 1);
  memcpy(buffer, rxbuf_.get(), size);
  if (withSleep)
  {
    osDelay(1);
  }
  return Result::OK;
}
