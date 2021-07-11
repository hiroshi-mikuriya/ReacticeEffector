/// @file      task/sound_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "sound_task.h"
#include "main.h"
#include "message/msglib.h"
#include <memory>

namespace
{
constexpr int32_t SIG_INITADC = 1 << 0;

constexpr uint32_t BLOCK_SIZE = 192;
constexpr uint32_t LR_BLOCK_SIZE = BLOCK_SIZE * 2;
constexpr uint32_t DIV = 0x80000000;

/// @brief float(-1.0f 〜 1.0f)に変換する
/// @param[in] src 入力音声
/// @param[out] left Left音声
/// @param[out] right Right音声
/// @param[in] size LRそれぞれの音声データ数
inline void toFloat(int32_t const *src, float *left, float *right, uint32_t size)
{
  for (uint32_t i = 0; i < size; ++i)
  {
    left[i] = static_cast<float>(src[i * 2]) / DIV;
    right[i] = static_cast<float>(src[i * 2 + 1]) / DIV;
  }
}
/// @brief int32に変換する
/// @param[in] left Left音声
/// @param[in] right Right音声
/// @param[out] dst 出力音声
/// @param[in] size LRそれぞれの音声データ数
inline void toInt32(float const *left, float const *right, int32_t *dst, uint32_t size)
{
  for (uint32_t i = 0; i < size; ++i)
  {
    dst[i * 2] = static_cast<int32_t>(left[i] * DIV);
    dst[i * 2 + 1] = static_cast<int32_t>(right[i] * DIV);
  }
}
/// @brief 音声処理
/// @param[in] effector エフェクター
/// @param[in] src 音声入力データ
/// @param[out] dst 音声出力データ
/// @param[in] left L音声計算用バッファ
/// @param[in] right R音声計算用バッファ
/// @param[in] size 音声データ数
void soundProc(satoh::msg::SOUND_EFFECTOR &effector, int32_t const *src, int32_t *dst, float *left, float *right, uint32_t size)
{
  LL_GPIO_SetOutputPin(TP13_GPIO_Port, TP13_Pin);
  toFloat(src, left, right, size);
  for (auto *fx : effector.fx)
  {
    if (fx)
    {
      fx->effect(left, right, size);
    }
  }
  toInt32(left, right, dst, size);
  LL_GPIO_ResetOutputPin(TP13_GPIO_Port, TP13_Pin);
}
} // namespace

void initPCM3060(satoh::I2C *i2c)
{
  LL_GPIO_ResetOutputPin(PCM_RST_GPIO_Port, PCM_RST_Pin);
  osDelay(100);
  LL_GPIO_SetOutputPin(PCM_RST_GPIO_Port, PCM_RST_Pin);
  constexpr uint8_t SLAVE_ADDR = 0x47 << 1;
  uint8_t v[] = {0x40, 0xC1}; // ADC/DACをNormalモードにする  S / E = 1 : Single End modeで使う
  if (i2c->write(SLAVE_ADDR, v, sizeof(v), true) == satoh::I2C::OK)
  {
    osSignalSet(soundTaskHandle, SIG_INITADC);
  }
}

void soundTaskProc(void const *argument)
{
  osSignalWait(SIG_INITADC, osWaitForever);
  if (satoh::addMsgTarget(4) != osOK)
  {
    return;
  }
  constexpr uint32_t SIZE = LR_BLOCK_SIZE * 2;
  std::unique_ptr<int32_t> rxbuf(new int32_t[SIZE]); // 音声信号受信バッファ配列 Lch前半 Lch後半 Rch前半 Rch後半
  std::unique_ptr<int32_t> txbuf(new int32_t[SIZE]); // 音声信号送信バッファ配列
  std::unique_ptr<float> left(new float[BLOCK_SIZE]);
  std::unique_ptr<float> right(new float[BLOCK_SIZE]);
  extern SAI_HandleTypeDef hsai_BlockA1;
  extern SAI_HandleTypeDef hsai_BlockB1;
  HAL_SAI_Transmit_DMA(&hsai_BlockB1, reinterpret_cast<uint8_t *>(txbuf.get()), SIZE);
  HAL_SAI_Receive_DMA(&hsai_BlockA1, reinterpret_cast<uint8_t *>(rxbuf.get()), SIZE);
  satoh::msg::SOUND_EFFECTOR effector{};
  for (;;)
  {
    auto res = satoh::recvMsg();
    auto *msg = res.msg();
    if (!msg)
    {
      continue;
    }
    switch (msg->type)
    {
    case satoh::msg::SOUND_DMA_HALF_NOTIFY:
      soundProc(effector, rxbuf.get(), txbuf.get(), left.get(), right.get(), BLOCK_SIZE);
      break;
    case satoh::msg::SOUND_DMA_CPLT_NOTIFY:
      soundProc(effector, rxbuf.get() + LR_BLOCK_SIZE, txbuf.get() + LR_BLOCK_SIZE, left.get(), right.get(), BLOCK_SIZE);
      break;
    case satoh::msg::SOUND_CHANGE_EFFECTOR_REQ:
      effector = *reinterpret_cast<satoh::msg::SOUND_EFFECTOR const *>(msg->bytes);
      break;
    }
  }
}

extern "C"
{
  /// @brief DMA半分受信コールバック
  /// @param[in] hsai SAI
  void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) { satoh::sendMsg(soundTaskHandle, satoh::msg::SOUND_DMA_HALF_NOTIFY); }
  /// @brief DMA全部受信コールバック
  /// @param[in] hsai SAI
  void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) { satoh::sendMsg(soundTaskHandle, satoh::msg::SOUND_DMA_CPLT_NOTIFY); }
}
