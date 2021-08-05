/// @file      peripheral/spi_master.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "spi_master.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_spi.h"
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
/// @param[in] dma DMA
/// @param[in] stream DMAストリーム
/// @param[in] size 通信データ量
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
} // namespace

satoh::SpiMaster::SpiMaster()
    : threadId_(0),     //
      sendOnly_(false), //
      spi_(0),          //
      dma_(0),          //
      txStream_(0),     //
      rxStream_(0)      //
{
}

satoh::SpiMaster::SpiMaster(osThreadId threadId,          //
                            SPI_TypeDef *spi,             //
                            DMA_TypeDef *dma,             //
                            uint32_t txStream,            //
                            uint32_t bufferSize) noexcept //
    : threadId_(threadId),                                //
      sendOnly_(true),                                    //
      spi_(spi),                                          //
      dma_(dma),                                          //
      txStream_(txStream),                                //
      rxStream_(0)                                        //
{
  LL_SPI_Enable(spi_);
  LL_DMA_EnableIT_TC(dma_, txStream_);
  LL_DMA_EnableIT_TE(dma_, txStream_);
  LL_SPI_EnableDMAReq_TX(spi_);
}
satoh::SpiMaster::SpiMaster(osThreadId threadId,          //
                            SPI_TypeDef *spi,             //
                            DMA_TypeDef *dma,             //
                            uint32_t txStream,            //
                            uint32_t rxStream,            //
                            uint32_t bufferSize) noexcept //
    : threadId_(threadId),                                //
      sendOnly_(false),                                   //
      spi_(spi),                                          //
      dma_(dma),                                          //
      txStream_(txStream),                                //
      rxStream_(rxStream)                                 //
{
  LL_SPI_Enable(spi_);
  LL_DMA_EnableIT_TC(dma_, rxStream_);
  LL_DMA_EnableIT_TE(dma_, rxStream_);
  LL_SPI_EnableDMAReq_TX(spi_);
  LL_SPI_EnableDMAReq_RX(spi_);
}
satoh::SpiMaster::SpiMaster(SpiMaster &&that) //
    : threadId_(that.threadId_),              //
      sendOnly_(that.sendOnly_),              //
      spi_(that.spi_),                        //
      dma_(that.dma_),                        //
      txStream_(that.txStream_),              //
      rxStream_(that.rxStream_)               //
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
    sendOnly_ = that.sendOnly_;
    spi_ = that.spi_;
    dma_ = that.dma_;
    txStream_ = that.txStream_;
    rxStream_ = that.rxStream_;
    that.spi_ = 0;
    that.dma_ = 0;
    that.~SpiMaster();
  }
  return *this;
}

satoh::SpiMaster::~SpiMaster()
{
  threadId_ = 0;
  sendOnly_ = false;
  if (dma_)
  {
    LL_DMA_DisableIT_TC(dma_, rxStream_);
    LL_DMA_DisableIT_TE(dma_, rxStream_);
    LL_DMA_DisableIT_TC(dma_, txStream_);
    LL_DMA_DisableIT_TE(dma_, txStream_);
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
}
satoh::SpiMaster::Result satoh::SpiMaster::send(uint8_t const *bytes, uint32_t size, uint32_t millisec) noexcept
{
  if (!spi_)
  {
    return ERROR;
  }
  if (LL_DMA_IsEnabledStream(dma_, txStream_))
  {
    return BUSY;
  }
  setDmaTransferSize(dma_, txStream_, size);
  LL_DMA_ConfigAddresses(dma_, txStream_,                   //
                         reinterpret_cast<uint32_t>(bytes), //
                         LL_SPI_DMA_GetRegAddr(spi_),       //
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH  //
  );
  clearSignals();
  LL_DMA_EnableStream(dma_, txStream_);
  osEvent ev = osSignalWait(satoh::SPI_MASTER_CLS_SIG_MASK, millisec);
  if (ev.status == osEventTimeout)
  {
    return TIMEOUT;
  }
  if (ev.status != osOK || (ev.value.signals & SIG_DMATXERR))
  {
    return ERROR;
  }
  return OK;
}

satoh::SpiMaster::Result satoh::SpiMaster::sendRecv(uint8_t const *tx, uint8_t *rx, uint32_t size, uint32_t millisec) noexcept
{
  if (!spi_)
  {
    return ERROR;
  }
  if (LL_DMA_IsEnabledStream(dma_, txStream_))
  {
    return BUSY;
  }
  setDmaTransferSize(dma_, txStream_, size);
  setDmaTransferSize(dma_, rxStream_, size);
  LL_DMA_ConfigAddresses(dma_, txStream_,                  //
                         reinterpret_cast<uint32_t>(tx),   //
                         LL_SPI_DMA_GetRegAddr(spi_),      //
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH //
  );
  LL_DMA_ConfigAddresses(dma_, rxStream_,                  //
                         LL_SPI_DMA_GetRegAddr(spi_),      //
                         reinterpret_cast<uint32_t>(rx),   //
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY //
  );
  clearSignals();
  LL_DMA_EnableStream(dma_, rxStream_);
  LL_DMA_EnableStream(dma_, txStream_);
  osEvent ev = osSignalWait(satoh::SPI_MASTER_CLS_SIG_MASK, millisec);
  if (ev.status == osEventTimeout)
  {
    return TIMEOUT;
  }
  if (ev.status != osOK || (ev.value.signals & SIG_DMATXERR))
  {
    return ERROR;
  }
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
