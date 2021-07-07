/// @file      task/sound_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "sound_task.h"
#include "main.h"
#include "message/msglib.h"
#include <cstring> // memcpy

namespace
{
constexpr int32_t SIG_INITADC = 1 << 0;

constexpr uint32_t BLOCK_SIZE = 16;
constexpr uint32_t LR_BLOCK_SIZE = BLOCK_SIZE * 2;
constexpr uint32_t DIV = 0x80000000;
int32_t s_rxbuf[LR_BLOCK_SIZE * 2] = {}; // 音声信号受信バッファ配列 Lch前半 Lch後半 Rch前半 Rch後半
int32_t s_txbuf[LR_BLOCK_SIZE * 2] = {}; // 音声信号送信バッファ配列

/// @brief 仮エフェクタークラス（後で消す）
class Effector
{
  float gain_;
  float th_;

  static void inRange(float min, float &v, float max)
  {
    v = std::max(min, v);
    v = std::min(max, v);
  }

public:
  Effector() : gain_(40.0f), th_(0.3f) {}
  virtual ~Effector() {}
  void effect(float (&v)[BLOCK_SIZE]) const noexcept
  {
    for (uint32_t i = 0; i < BLOCK_SIZE; ++i)
    {
      v[i] *= gain_;
      inRange(-th_, v[i], th_);
    }
  }
  void setGain(bool up) noexcept
  {
    if (up)
    {
      gain_ += 1.0f;
    }
    else
    {
      gain_ -= 1.0f;
    }
    inRange(1.0f, gain_, 100.0f);
  }
  void setTh(bool up) noexcept
  {
    if (up)
    {
      th_ += 0.05f;
    }
    else
    {
      th_ -= 0.05f;
    }
    inRange(0.05f, th_, 1.0f);
  }
} s_fx;
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
  s_fx.effect(right);
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
  satoh::addMsgTarget(4);
  extern SAI_HandleTypeDef hsai_BlockA1;
  extern SAI_HandleTypeDef hsai_BlockB1;
  HAL_SAI_Transmit_DMA(&hsai_BlockB1, reinterpret_cast<uint8_t *>(s_txbuf), BLOCK_SIZE * 4);
  HAL_SAI_Receive_DMA(&hsai_BlockA1, reinterpret_cast<uint8_t *>(s_rxbuf), BLOCK_SIZE * 4);
  for (;;)
  {
    auto res = satoh::recvMsg();
    auto *msg = res.msg();
    if (!msg)
    {
      continue;
    }
    if (msg->type == satoh::msg::ROTARY_ENCODER_NOTIFY)
    {
      auto *param = reinterpret_cast<satoh::msg::ROTARY_ENCODER const *>(msg->bytes);
      if (0 < param->angleDiff[0])
      {
        s_fx.setGain(true);
      }
      if (param->angleDiff[0] < 0)
      {
        s_fx.setGain(false);
      }
      if (0 < param->angleDiff[1])
      {
        s_fx.setTh(true);
      }
      if (param->angleDiff[1] < 0)
      {
        s_fx.setTh(false);
      }
    }
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
