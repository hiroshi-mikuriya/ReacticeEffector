/// @file      task/sound_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "sound_task.h"
#include "main.h"
#include <cstring> // memcpy

namespace
{
constexpr int32_t SIG_INITADC = 1 << 0;
constexpr uint32_t BLOCK_SIZE = 16;
int32_t rxbuf[BLOCK_SIZE * 4] = {}; // 音声信号受信バッファ配列 Lch前半 Lch後半 Rch前半 Rch後半
int32_t txbuf[BLOCK_SIZE * 4] = {}; // 音声信号送信バッファ配列
} // namespace

void initPCM3060(satoh::I2C *i2c)
{
  LL_GPIO_ResetOutputPin(PCM_RST_GPIO_Port, PCM_RST_Pin);
  osDelay(100);
  LL_GPIO_SetOutputPin(PCM_RST_GPIO_Port, PCM_RST_Pin);
  constexpr uint8_t SLAVE_ADDR = 0x47 << 1;
  uint8_t v[] = {0x40, 0xC1}; // ADC/DACをNormalモードにする  S / E = 1 : Single End modeで使う
  if (i2c->write(SLAVE_ADDR, v, sizeof(v)) == satoh::I2C::OK)
  {
    osSignalSet(soundTaskHandle, SIG_INITADC);
  }
}

void soundTaskProc(void const *argument)
{
  osSignalWait(SIG_INITADC, osWaitForever);
  extern SAI_HandleTypeDef hsai_BlockA1;
  extern SAI_HandleTypeDef hsai_BlockB1;
  HAL_SAI_Transmit_DMA(&hsai_BlockB1, reinterpret_cast<uint8_t *>(txbuf), BLOCK_SIZE * 4);
  HAL_SAI_Receive_DMA(&hsai_BlockA1, reinterpret_cast<uint8_t *>(rxbuf), BLOCK_SIZE * 4);
  for (;;)
  {
    osDelay(10);
  }
}

extern "C"
{
  /// @brief DMA半分受信コールバック
  /// @param[in] hsai SAI
  void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
  {
    uint32_t s = sizeof(rxbuf) / 2;
    memcpy(txbuf, rxbuf, s);
    // TODO
  }
  /// @brief DMA全部受信コールバック
  /// @param[in] hsai SAI
  void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
  {
    uint32_t s = sizeof(rxbuf) / 2;
    memcpy(txbuf + BLOCK_SIZE * 2, rxbuf + BLOCK_SIZE * 2, s);
    // TODO
  }
}
