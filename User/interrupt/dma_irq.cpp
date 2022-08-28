/// @file      interrupt/dma_irq.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2022 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "main.h"

extern "C"
{
  /// @brief DMA1 Stream0 割り込み
  void DMA1_Stream0_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC0(DMA1))
    {
      LL_DMA_ClearFlag_TC0(DMA1);
      extern void i2cRxEndIRQ(void);
      i2cRxEndIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE0(DMA1))
    {
      LL_DMA_ClearFlag_TE0(DMA1);
      extern void i2cRxErrorIRQ(void);
      i2cRxErrorIRQ();
    }
  }

  /// @brief DMA1 Stream1 割り込み
  void DMA1_Stream1_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC1(DMA1))
    {
      LL_DMA_ClearFlag_TC1(DMA1);
    }
    if (LL_DMA_IsActiveFlag_TE1(DMA1))
    {
      LL_DMA_ClearFlag_TE1(DMA1);
    }
  }

  /// @brief DMA1 Stream2 割り込み
  void DMA1_Stream2_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC2(DMA1))
    {
      LL_DMA_ClearFlag_TC2(DMA1);
    }
    if (LL_DMA_IsActiveFlag_TE2(DMA1))
    {
      LL_DMA_ClearFlag_TE2(DMA1);
    }
  }

  /// @brief DMA1 Stream3 割り込み
  void DMA1_Stream3_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC3(DMA1))
    {
      LL_DMA_ClearFlag_TC3(DMA1);
      extern void spiSramRxEndIRQ(void);
      spiSramRxEndIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE3(DMA1))
    {
      LL_DMA_ClearFlag_TE3(DMA1);
      extern void spiSramRxErrorIRQ(void);
      spiSramRxErrorIRQ();
    }
  }

  /// @brief DMA1 Stream4 割り込み
  void DMA1_Stream4_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC4(DMA1))
    {
      LL_DMA_ClearFlag_TC4(DMA1);
      extern void spiSramTxEndIRQ(void);
      spiSramTxEndIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE4(DMA1))
    {
      LL_DMA_ClearFlag_TE4(DMA1);
      extern void spiSramTxErrorIRQ(void);
      spiSramTxErrorIRQ();
    }
  }

  /// @brief DMA1 Stream5 割り込み
  void DMA1_Stream5_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC5(DMA1))
    {
      LL_DMA_ClearFlag_TC5(DMA1);
      extern void neoPixelTxEndIRQ(void);
      neoPixelTxEndIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE5(DMA1))
    {
      LL_DMA_ClearFlag_TE5(DMA1);
      extern void neoPixelTxErrorIRQ(void);
      neoPixelTxErrorIRQ();
    }
  }

  /// @brief DMA1 Stream6 割り込み
  void DMA1_Stream6_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC6(DMA1))
    {
      LL_DMA_ClearFlag_TC6(DMA1);
      extern void uart2TxEndIRQ(void);
      uart2TxEndIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE6(DMA1))
    {
      LL_DMA_ClearFlag_TE6(DMA1);
      extern void uart2TxErrorIRQ(void);
      uart2TxErrorIRQ();
    }
  }

  /// @brief DMA1 Stream7 割り込み
  void DMA1_Stream7_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC7(DMA1))
    {
      LL_DMA_ClearFlag_TC7(DMA1);
      extern void i2cTxEndIRQ(void);
      i2cTxEndIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE7(DMA1))
    {
      LL_DMA_ClearFlag_TE7(DMA1);
      extern void i2cTxErrorIRQ(void);
      i2cTxErrorIRQ();
    }
  }

  /// @brief DMA2 Stream0 割り込み
  void DMA2_Stream0_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC0(DMA2))
    {
      LL_DMA_ClearFlag_TC0(DMA2);
      // TODO SPI4 rx end IRQ
    }
    if (LL_DMA_IsActiveFlag_TE0(DMA2))
    {
      LL_DMA_ClearFlag_TE0(DMA2);
      // TODO SPI4 rx error IRQ
    }
  }

  /// @brief DMA2 Stream1 割り込み
  void DMA2_Stream1_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC1(DMA2))
    {
      LL_DMA_ClearFlag_TC1(DMA2);
      // TODO SPI4 tx end IRQ
    }
    if (LL_DMA_IsActiveFlag_TE1(DMA2))
    {
      LL_DMA_ClearFlag_TE1(DMA2);
      // TODO SPI4 tx error IRQ
    }
  }

  /// @brief DMA2 Stream2 割り込み
  void DMA2_Stream2_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC2(DMA2))
    {
      LL_DMA_ClearFlag_TC2(DMA2);
    }
    if (LL_DMA_IsActiveFlag_TE2(DMA2))
    {
      LL_DMA_ClearFlag_TE2(DMA2);
    }
  }

  /// @brief DMA2 Stream3 割り込み
  void DMA2_Stream3_IRQHandler(void)
  {
    extern DMA_HandleTypeDef hdma_sai1_a;
    HAL_DMA_IRQHandler(&hdma_sai1_a);
  }

  /// @brief DMA2 Stream4 割り込み
  void DMA2_Stream4_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC4(DMA2))
    {
      LL_DMA_ClearFlag_TC4(DMA2);
      extern void adc1CpltIRQ(void);
      adc1CpltIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE4(DMA2))
    {
      LL_DMA_ClearFlag_TE4(DMA2);
      extern void adc1ErrorIRQ(void);
      adc1ErrorIRQ();
    }
  }

  /// @brief DMA2 Stream5 割り込み
  void DMA2_Stream5_IRQHandler(void)
  {
    extern DMA_HandleTypeDef hdma_sai1_b;
    HAL_DMA_IRQHandler(&hdma_sai1_b);
  }

  /// @brief DMA2 Stream6 割り込み
  void DMA2_Stream6_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC6(DMA2))
    {
      LL_DMA_ClearFlag_TC6(DMA2);
    }
    if (LL_DMA_IsActiveFlag_TE6(DMA2))
    {
      LL_DMA_ClearFlag_TE6(DMA2);
    }
  }

  /// @brief DMA2 Stream7 割り込み
  void DMA2_Stream7_IRQHandler(void)
  {
    if (LL_DMA_IsActiveFlag_TC7(DMA2))
    {
      LL_DMA_ClearFlag_TC7(DMA2);
      extern void uart1TxEndIRQ(void);
      uart1TxEndIRQ();
    }
    if (LL_DMA_IsActiveFlag_TE7(DMA2))
    {
      LL_DMA_ClearFlag_TE7(DMA2);
      extern void uart1TxErrorIRQ(void);
      uart1TxErrorIRQ();
    }
  }
}