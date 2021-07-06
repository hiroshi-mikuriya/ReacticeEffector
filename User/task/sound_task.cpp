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
constexpr uint32_t LR_BLOCK_SIZE = BLOCK_SIZE * 2;
constexpr uint32_t DIV = 0x80000000;
int32_t s_rxbuf[LR_BLOCK_SIZE * 2] = {}; // 音声信号受信バッファ配列 Lch前半 Lch後半 Rch前半 Rch後半
int32_t s_txbuf[LR_BLOCK_SIZE * 2] = {}; // 音声信号送信バッファ配列
/// @brief float(-1.0f 〜 1.0f)に変換する
/// @param[in] src 入力音声
/// @param[out] left Left音声
/// @param[out] right Right音声
inline void toFloat(int32_t const *src, float (&left)[BLOCK_SIZE], float (&right)[BLOCK_SIZE])
{
  for (uint32_t i = 0; i < BLOCK_SIZE; ++i)
  {
    left[i] = static_cast<float>(src[i * 2]) / DIV;
    right[i] = static_cast<float>(src[i * 2 + 1]) / DIV;
  }
}
/// @brief int32に変換する
/// @param[in] left Left音声
/// @param[in] right Right音声
/// @param[out] dst 出力音声
inline void toInt32(float const (&left)[BLOCK_SIZE], float const (&right)[BLOCK_SIZE], int32_t *dst)
{
  for (uint32_t i = 0; i < BLOCK_SIZE; ++i)
  {
    dst[i * 2] = static_cast<int32_t>(left[i] * DIV);
    dst[i * 2 + 1] = static_cast<int32_t>(right[i] * DIV);
  }
}
/// @brief 音声処理
/// @param[in] src 音声入力データ
/// @param[out] dst 音声出力データ
void soundProc(int32_t const *src, int32_t *dst)
{
  static float left[BLOCK_SIZE];
  static float right[BLOCK_SIZE];
  toFloat(src, left, right);
  // TODO ここでエフェクト処理
  toInt32(left, right, dst);
}
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
  HAL_SAI_Transmit_DMA(&hsai_BlockB1, reinterpret_cast<uint8_t *>(s_txbuf), BLOCK_SIZE * 4);
  HAL_SAI_Receive_DMA(&hsai_BlockA1, reinterpret_cast<uint8_t *>(s_rxbuf), BLOCK_SIZE * 4);
  for (;;)
  {
    osDelay(10);
  }
}

extern "C"
{
  /// @brief DMA半分受信コールバック
  /// @param[in] hsai SAI
  void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) { soundProc(s_rxbuf, s_txbuf); }
  /// @brief DMA全部受信コールバック
  /// @param[in] hsai SAI
  void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) { soundProc(s_rxbuf + LR_BLOCK_SIZE, s_txbuf + LR_BLOCK_SIZE); }
}
