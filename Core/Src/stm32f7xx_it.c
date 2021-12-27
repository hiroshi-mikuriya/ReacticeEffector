/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_sai1_a;
extern DMA_HandleTypeDef hdma_sai1_b;
extern TIM_HandleTypeDef htim7;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M7 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC0(DMA1))
  {
    LL_DMA_ClearFlag_TC0(DMA1);
    extern void i2cRxEndIRQ(void);
    i2cRxEndIRQ();
  }
  /* USER CODE END DMA1_Stream0_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE0(DMA1))
  {
    LL_DMA_ClearFlag_TE0(DMA1);
    extern void i2cRxErrorIRQ(void);
    i2cRxErrorIRQ();
  }
  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC3(DMA1))
  {
    LL_DMA_ClearFlag_TC3(DMA1);
    extern void spiSramRxEndIRQ(void);
    spiSramRxEndIRQ();
  }
  /* USER CODE END DMA1_Stream3_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE3(DMA1))
  {
    LL_DMA_ClearFlag_TE3(DMA1);
    extern void spiSramRxErrorIRQ(void);
    spiSramRxErrorIRQ();
  }
  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream4 global interrupt.
  */
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream4_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC4(DMA1))
  {
    LL_DMA_ClearFlag_TC4(DMA1);
    extern void spiSramTxEndIRQ(void);
    spiSramTxEndIRQ();
  }
  /* USER CODE END DMA1_Stream4_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream4_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE4(DMA1))
  {
    LL_DMA_ClearFlag_TE4(DMA1);
    extern void spiSramTxErrorIRQ(void);
    spiSramTxErrorIRQ();
  }
  /* USER CODE END DMA1_Stream4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC5(DMA1))
  {
    LL_DMA_ClearFlag_TC5(DMA1);
    extern void neoPixelTxEndIRQ(void);
    neoPixelTxEndIRQ();
  }
  /* USER CODE END DMA1_Stream5_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE5(DMA1))
  {
    LL_DMA_ClearFlag_TE5(DMA1);
    extern void neoPixelTxErrorIRQ(void);
    neoPixelTxErrorIRQ();
  }
  /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC6(DMA1))
  {
    LL_DMA_ClearFlag_TC6(DMA1);
    extern void uart2TxEndIRQ(void);
    uart2TxEndIRQ();
  }
  /* USER CODE END DMA1_Stream6_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE6(DMA1))
  {
    LL_DMA_ClearFlag_TE6(DMA1);
    extern void uart2TxErrorIRQ(void);
    uart2TxErrorIRQ();
  }
  /* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    /* USER CODE BEGIN LL_EXTI_LINE_5 */
    extern void extiSw2IRQ(void);
    extiSw2IRQ();
    /* USER CODE END LL_EXTI_LINE_5 */
  }
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
    /* USER CODE BEGIN LL_EXTI_LINE_6 */
    extern void extiMpuIRQ(void);
    extiMpuIRQ();
    /* USER CODE END LL_EXTI_LINE_6 */
  }
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_7) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_7);
    /* USER CODE BEGIN LL_EXTI_LINE_7 */
    extern void extiSwIRQ(void);
    extiSwIRQ();
    /* USER CODE END LL_EXTI_LINE_7 */
  }
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */
  if (LL_TIM_IsActiveFlag_UPDATE(TIM10))
  {
    LL_TIM_ClearFlag_UPDATE(TIM10);
    extern void adc1TimIRQ(void);
    extern void appTimIRQ(void);
    adc1TimIRQ();
    appTimIRQ();
  }
  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */

  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */
  extern void i2cEvIRQ(void);
  i2cEvIRQ();
  /* USER CODE END I2C1_EV_IRQn 0 */

  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */
  extern void i2cErIRQ(void);
  i2cErIRQ();
  /* USER CODE END I2C1_ER_IRQn 0 */

  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  extern void uart1RxIRQ(void);
  uart1RxIRQ();
  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
  extern void uart2RxIRQ(void);
  uart2RxIRQ();
  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_11) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
    /* USER CODE BEGIN LL_EXTI_LINE_11 */
    // RPI SPI NSS
    /* USER CODE END LL_EXTI_LINE_11 */
  }
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream7 global interrupt.
  */
void DMA1_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream7_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC7(DMA1))
  {
    LL_DMA_ClearFlag_TC7(DMA1);
    extern void i2cTxEndIRQ(void);
    i2cTxEndIRQ();
  }
  /* USER CODE END DMA1_Stream7_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream7_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE7(DMA1))
  {
    LL_DMA_ClearFlag_TE7(DMA1);
    extern void i2cTxErrorIRQ(void);
    i2cTxErrorIRQ();
  }
  /* USER CODE END DMA1_Stream7_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
  if (LL_TIM_IsActiveFlag_UPDATE(TIM6))
  {
    LL_TIM_ClearFlag_UPDATE(TIM6);
    LL_GPIO_TogglePin(LED_ACT_GPIO_Port, LED_ACT_Pin);
  }
  /* USER CODE END TIM6_DAC_IRQn 0 */

  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC0(DMA2))
  {
    LL_DMA_ClearFlag_TC0(DMA2);
    // TODO SPI4 rx end IRQ
  }
  /* USER CODE END DMA2_Stream0_IRQn 0 */

  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE0(DMA2))
  {
    LL_DMA_ClearFlag_TE0(DMA2);
    // TODO SPI4 rx error IRQ
  }
  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream1 global interrupt.
  */
void DMA2_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC1(DMA2))
  {
    LL_DMA_ClearFlag_TC1(DMA2);
    // TODO SPI4 tx end IRQ
  }
  /* USER CODE END DMA2_Stream1_IRQn 0 */

  /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE1(DMA2))
  {
    LL_DMA_ClearFlag_TE1(DMA2);
    // TODO SPI4 tx error IRQ
  }
  /* USER CODE END DMA2_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sai1_a);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream4 global interrupt.
  */
void DMA2_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream4_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC4(DMA2))
  {
    LL_DMA_ClearFlag_TC4(DMA2);
    extern void adc1CpltIRQ(void);
    adc1CpltIRQ();
  }
  /* USER CODE END DMA2_Stream4_IRQn 0 */

  /* USER CODE BEGIN DMA2_Stream4_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE4(DMA2))
  {
    LL_DMA_ClearFlag_TE4(DMA2);
    extern void adc1ErrorIRQ(void);
    adc1ErrorIRQ();
  }
  /* USER CODE END DMA2_Stream4_IRQn 1 */
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */

  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream5 global interrupt.
  */
void DMA2_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream5_IRQn 0 */

  /* USER CODE END DMA2_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sai1_b);
  /* USER CODE BEGIN DMA2_Stream5_IRQn 1 */

  /* USER CODE END DMA2_Stream5_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */
  if (LL_DMA_IsActiveFlag_TC7(DMA2))
  {
    LL_DMA_ClearFlag_TC7(DMA2);
    extern void uart1TxEndIRQ(void);
    uart1TxEndIRQ();
  }
  /* USER CODE END DMA2_Stream7_IRQn 0 */

  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */
  if (LL_DMA_IsActiveFlag_TE7(DMA2))
  {
    LL_DMA_ClearFlag_TE7(DMA2);
    extern void uart1TxErrorIRQ(void);
    uart1TxErrorIRQ();
  }
  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

