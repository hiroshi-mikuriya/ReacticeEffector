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
constexpr int32_t SIG_DMAEND = (1 << 0) & satoh::SPI_MASTER_CLS_SIG_MASK;
constexpr int32_t SIG_DMAERR = (1 << 1) & satoh::SPI_MASTER_CLS_SIG_MASK;
} // namespace

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
      txbuf_(bufferSize),                                 //
      rxStream_(0),                                       //
      rxbuf_()                                            //
{
  LL_SPI_Enable(spi_);
  LL_DMA_ConfigAddresses(dma_, txStream_,                          //
                         reinterpret_cast<uint32_t>(txbuf_.get()), //
                         LL_SPI_DMA_GetRegAddr(spi_),              //
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH         //
  );
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
      txbuf_(bufferSize),                                 //
      rxStream_(rxStream),                                //
      rxbuf_(bufferSize)                                  //
{
  LL_SPI_Enable(spi_);
  LL_DMA_ConfigAddresses(dma_, txStream_,                          //
                         reinterpret_cast<uint32_t>(txbuf_.get()), //
                         LL_SPI_DMA_GetRegAddr(spi_),              //
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH         //
  );
  LL_DMA_ConfigAddresses(dma_, rxStream_,                          //
                         LL_SPI_DMA_GetRegAddr(spi_),              //
                         reinterpret_cast<uint32_t>(rxbuf_.get()), //
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY         //
  );
  LL_DMA_EnableIT_TC(dma_, rxStream_);
  LL_DMA_EnableIT_TE(dma_, rxStream_);
  LL_SPI_EnableDMAReq_TX(spi_);
  LL_SPI_EnableDMAReq_RX(spi_);
}
satoh::SpiMaster::~SpiMaster()
{
  LL_SPI_DisableDMAReq_RX(spi_);
  LL_SPI_DisableDMAReq_TX(spi_);
  LL_DMA_DisableIT_TC(dma_, rxStream_);
  LL_DMA_DisableIT_TE(dma_, rxStream_);
  LL_DMA_DisableIT_TC(dma_, txStream_);
  LL_DMA_DisableIT_TE(dma_, txStream_);
  LL_SPI_Disable(spi_);
}
satoh::SpiMaster::Result satoh::SpiMaster::send(uint8_t const *bytes, uint32_t size, uint32_t millisec) noexcept
{
  if (LL_DMA_IsEnabledStream(dma_, txStream_))
  {
    return BUSY;
  }
  switch (LL_DMA_GetPeriphSize(dma_, txStream_))
  {
  case LL_DMA_PDATAALIGN_BYTE:
    LL_DMA_SetDataLength(dma_, txStream_, size);
    break;
  case LL_DMA_PDATAALIGN_HALFWORD:
    LL_DMA_SetDataLength(dma_, txStream_, size / 2);
    break;
  case LL_DMA_PDATAALIGN_WORD:
    LL_DMA_SetDataLength(dma_, txStream_, size / 4);
    break;
  }
  memcpy(txbuf_.get(), bytes, size);
  LL_DMA_EnableStream(dma_, txStream_);
  osEvent ev = osSignalWait(satoh::SPI_MASTER_CLS_SIG_MASK, millisec);
  if (ev.status == osEventTimeout)
  {
    return TIMEOUT;
  }
  if (ev.status != osOK || (ev.value.signals & SIG_DMAERR))
  {
    return ERROR;
  }
  return OK;
}

void satoh::SpiMaster::notifyTxEndIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMAEND);
}

void satoh::SpiMaster::notifyTxErrorIRQ() noexcept
{
  osSignalSet(threadId_, SIG_DMAERR);
}
