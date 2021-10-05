/// @file      peripheral/spi_master.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "spi_master.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_spi.h"
#include <mutex>
#include <string.h> // memset

namespace
{
constexpr int32_t SIG_DMATXEND = (1 << 0) & satoh::SPI_MASTER_CLS_SIG_MASK;
constexpr int32_t SIG_DMATXERR = (1 << 1) & satoh::SPI_MASTER_CLS_SIG_MASK;
constexpr int32_t SIG_DMARXEND = (1 << 2) & satoh::SPI_MASTER_CLS_SIG_MASK;
constexpr int32_t SIG_DMARXERR = (1 << 3) & satoh::SPI_MASTER_CLS_SIG_MASK;
/// @brief シグナルをクリアする
void clearSignals()
{
  osSignalWait(satoh::SPI_MASTER_CLS_SIG_MASK, 0);
}
/// @brief DMA通信サイズを設定する
/// @param [in] dma DMA
/// @param [in] stream DMAストリーム
/// @param [in] size 通信データ量
void setDmaTransferSize(DMA_TypeDef *dma, uint32_t stream, uint32_t size)
{
  switch (LL_DMA_GetPeriphSize(dma, stream))
  {
  case LL_DMA_PDATAALIGN_BYTE:
    LL_DMA_SetDataLength(dma, stream, size);
    break;
  case LL_DMA_PDATAALIGN_HALFWORD:
    LL_DMA_SetDataLength(dma, stream, size / 2);
    break;
  case LL_DMA_PDATAALIGN_WORD:
    LL_DMA_SetDataLength(dma, stream, size / 4);
    break;
  }
}
/// @brief NSSレベルをコンストラクタ・デストラクタで変更する
class NSS
{
  GPIO_TypeDef *gpio_; ///< GPIO
  uint32_t pin_;       ///< ピン

public:
  /// @brief コンストラクタ @param[in] gpio GPIO @param[in] pin ピン
  explicit NSS(GPIO_TypeDef *gpio, uint32_t pin) : gpio_(gpio), pin_(pin)
  {
    if (gpio_)
    {
      LL_GPIO_ResetOutputPin(gpio_, pin_);
    }
  }
  /// @brief デストラクタ
  virtual ~NSS()
  {
    if (gpio_)
    {
      LL_GPIO_SetOutputPin(gpio_, pin_);
    }
  }
};
} // namespace

satoh::SpiMaster::SpiMaster()
    : sendOnly_(false), //
      spi_(0),          //
      dma_(0),          //
      txStream_(0),     //
      rxStream_(0),     //
      nssGpio_(0),      //
      nssPin_(0)        //
{
}

satoh::SpiMaster::SpiMaster(SPI_TypeDef *spi,           //
                            DMA_TypeDef *dma,           //
                            uint32_t txStream) noexcept //
    : threadId_(0),                                     //
      sendOnly_(true),                                  //
      spi_(spi),                                        //
      dma_(dma),                                        //
      txStream_(txStream),                              //
      rxStream_(0),                                     //
      nssGpio_(0),                                      //
      nssPin_(0)                                        //
{
  LL_SPI_Enable(spi_);
  LL_DMA_EnableIT_TC(dma_, txStream_);
  LL_DMA_EnableIT_TE(dma_, txStream_);
  LL_SPI_EnableDMAReq_TX(spi_);
}
satoh::SpiMaster::SpiMaster(SPI_TypeDef *spi,         //
                            DMA_TypeDef *dma,         //
                            uint32_t txStream,        //
                            uint32_t rxStream,        //
                            GPIO_TypeDef *nssGpio,    //
                            uint32_t nssPin) noexcept //
    : threadId_(0),                                   //
      sendOnly_(false),                               //
      spi_(spi),                                      //
      dma_(dma),                                      //
      txStream_(txStream),                            //
      rxStream_(rxStream),                            //
      nssGpio_(nssGpio),                              //
      nssPin_(nssPin)                                 //
{
  LL_SPI_Enable(spi_);
  LL_DMA_EnableIT_TC(dma_, txStream_);
  LL_DMA_EnableIT_TE(dma_, txStream_);
  LL_DMA_EnableIT_TC(dma_, rxStream_);
  LL_DMA_EnableIT_TE(dma_, rxStream_);
  LL_SPI_EnableDMAReq_TX(spi_);
  LL_SPI_EnableDMAReq_RX(spi_);
  NSS(nssGpio_, nssPin_);
}
satoh::SpiMaster::SpiMaster(SpiMaster &&that) //
    : threadId_(that.threadId_),              //
      mutex_(std::move(that.mutex_)),         //
      sendOnly_(that.sendOnly_),              //
      spi_(that.spi_),                        //
      dma_(that.dma_),                        //
      txStream_(that.txStream_),              //
      rxStream_(that.rxStream_),              //
      nssGpio_(that.nssGpio_),                //
      nssPin_(that.nssPin_)                   //
{
  that.spi_ = 0;
  that.dma_ = 0;
  that.~SpiMaster();
}
satoh::SpiMaster &satoh::SpiMaster::operator=(SpiMaster &&that)
{
  if (this != &that)
  {
    this->~SpiMaster();
    threadId_ = that.threadId_;
    mutex_ = std::move(that.mutex_);
    sendOnly_ = that.sendOnly_;
    spi_ = that.spi_;
    dma_ = that.dma_;
    txStream_ = that.txStream_;
    rxStream_ = that.rxStream_;
    nssGpio_ = that.nssGpio_;
    nssPin_ = that.nssPin_;
    that.spi_ = 0;
    that.dma_ = 0;
    that.~SpiMaster();
  }
  return *this;
}

satoh::SpiMaster::~SpiMaster()
{
  threadId_ = 0;
  mutex_.remove();
  sendOnly_ = false;
  if (dma_)
  {
    LL_DMA_DisableIT_TC(dma_, rxStream_);
    LL_DMA_DisableIT_TE(dma_, rxStream_);
    LL_DMA_DisableStream(dma_, rxStream_);
    LL_DMA_DisableIT_TC(dma_, txStream_);
    LL_DMA_DisableIT_TE(dma_, txStream_);
    LL_DMA_DisableStream(dma_, txStream_);
    dma_ = 0;
  }
  rxStream_ = 0;
  txStream_ = 0;
  if (spi_)
  {
    LL_SPI_DisableDMAReq_RX(spi_);
    LL_SPI_DisableDMAReq_TX(spi_);
    LL_SPI_Disable(spi_);
    spi_ = 0;
  }
  nssGpio_ = 0;
  nssPin_ = 0;
}

/// @brief DMAを使って通信する
/// @param [in] dma DMA
/// @param [in] stream DMAストリーム
/// @param [in] sigend 通信成功シグナル
/// @param [in] sigerr 通信失敗シグナル
/// @param [in] tm 通信タイムアウト（ミリ秒）
#define TRANSFER(dma, stream, sigend, sigerr, tm)           \
  do                                                        \
  {                                                         \
    LL_DMA_EnableStream((dma), (stream));                   \
    osEvent ev = osSignalWait((sigend) | (sigerr), (tm));   \
    if (ev.status == osEventTimeout)                        \
    {                                                       \
      return TIMEOUT;                                       \
    }                                                       \
    if (ev.status != osOK || (ev.value.signals & (sigerr))) \
    {                                                       \
      return ERROR;                                         \
    }                                                       \
  } while (0)

satoh::SpiMaster::Result satoh::SpiMaster::send(void const *bytes, uint32_t size, uint32_t millisec) const noexcept
{
  std::lock_guard<Mutex> lock(mutex_);
  if (!spi_)
  {
    return ERROR;
  }
  if (LL_DMA_IsEnabledStream(dma_, txStream_))
  {
    return BUSY;
  }
  threadId_ = osThreadGetId();
  if (threadId_ == 0)
  {
    return Result::ERROR;
  }
  setDmaTransferSize(dma_, txStream_, size);
  LL_DMA_ConfigAddresses(dma_, txStream_,                   //
                         reinterpret_cast<uint32_t>(bytes), //
                         LL_SPI_DMA_GetRegAddr(spi_),       //
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH  //
  );
  clearSignals();
  NSS nss(nssGpio_, nssPin_);
  TRANSFER(dma_, txStream_, SIG_DMATXEND, SIG_DMATXERR, millisec);
  return OK;
}

satoh::SpiMaster::Result satoh::SpiMaster::sendRecv(void const *tbytes, void *rbytes, uint32_t size, uint32_t millisec) const noexcept
{
  std::lock_guard<Mutex> lock(mutex_);
  if (!spi_ || sendOnly_)
  {
    return ERROR;
  }
  if (LL_DMA_IsEnabledStream(dma_, txStream_) || LL_DMA_IsEnabledStream(dma_, rxStream_))
  {
    return BUSY;
  }
  threadId_ = osThreadGetId();
  if (threadId_ == 0)
  {
    return Result::ERROR;
  }
  setDmaTransferSize(dma_, txStream_, size);
  setDmaTransferSize(dma_, rxStream_, size);
  LL_DMA_ConfigAddresses(dma_, txStream_,                    //
                         reinterpret_cast<uint32_t>(tbytes), //
                         LL_SPI_DMA_GetRegAddr(spi_),        //
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH   //
  );
  LL_DMA_ConfigAddresses(dma_, rxStream_,                    //
                         LL_SPI_DMA_GetRegAddr(spi_),        //
                         reinterpret_cast<uint32_t>(rbytes), //
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY   //
  );
  clearSignals();
  NSS nss(nssGpio_, nssPin_);
  LL_DMA_EnableStream(dma_, rxStream_);
  TRANSFER(dma_, txStream_, SIG_DMATXEND, SIG_DMATXERR, millisec);
  return OK;
}

void satoh::SpiMaster::notifyTxEndIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMATXEND);
}

void satoh::SpiMaster::notifyTxErrorIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMATXERR);
}

void satoh::SpiMaster::notifyRxEndIRQ() noexcept
{
  // osSignalSet(threadId_, SIG_DMARXEND);
}

void satoh::SpiMaster::notifyRxErrorIRQ() noexcept
{
  // osSignalSet(threadId_, SIG_DMARXERR);
}
