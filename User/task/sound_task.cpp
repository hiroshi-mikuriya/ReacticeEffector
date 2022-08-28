/// @file      task/sound_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "common/alloc.hpp"
#include "common/dma_mem.h"
#include "effector/pop_noise_reductor.hpp"
#include "handles.h"
#include "main.h"
#include "message/type.h"
#include "peripheral/i2c.h"

namespace fx = satoh::fx;
namespace msg = satoh::msg;

namespace
{
/// DAC初期化完了イベント
constexpr int32_t SIG_INITADC = 1 << 0;
/// 音声信号を整数・浮動小数変換するための係数
constexpr uint32_t DIV = 0x80000000;

/// @brief float(-1.0f 〜 1.0f)に変換する
/// @param [in] src 入力音声
/// @param [out] left Left音声
/// @param [out] right Right音声
/// @param [in] size LRそれぞれの音声データ数
inline void toFloat(int32_t const *src, float *left, float *right, uint32_t size)
{
  for (uint32_t i = 0; i < size; ++i)
  {
    left[i] = static_cast<float>(src[i * 2]) / DIV;
    right[i] = static_cast<float>(src[i * 2 + 1]) / DIV;
  }
}
/// @brief int32に変換する
/// @param [in] left Left音声
/// @param [in] right Right音声
/// @param [out] dst 出力音声
/// @param [in] size LRそれぞれの音声データ数
inline void toInt32(float const *left, float const *right, int32_t *dst, uint32_t size)
{
  for (uint32_t i = 0; i < size; ++i)
  {
    dst[i * 2] = static_cast<int32_t>(left[i] * DIV);
    dst[i * 2 + 1] = static_cast<int32_t>(right[i] * DIV);
  }
}
/// @brief 音声処理
/// @param [in] effector エフェクター
/// @param [in] pop ポップノイズ除去
/// @param [in] src 音声入力データ
/// @param [out] dst 音声出力データ
/// @param [in] left L音声計算用バッファ
/// @param [in] right R音声計算用バッファ
/// @param [in] size 音声データ数
void soundProc(msg::SOUND_EFFECTOR &effector, fx::PopNoiseReductor &pop, int32_t const *src, int32_t *dst, float *left, float *right, uint32_t size)
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
  pop.reduct(left, right, size);
  toInt32(left, right, dst, size);
  LL_GPIO_ResetOutputPin(TP13_GPIO_Port, TP13_Pin);
}
} // namespace

extern "C"
{
  /// @brief PCM3060初期化
  /// @param [in] i2c I2C通信クラス
  /// @note I2Cタスクから呼ぶこと
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
  /// @brief soundTask内部処理
  /// @param [in] argument 未使用
  void soundTaskProc(void const *argument)
  {
    UNUSED(argument);
    osEvent ev = osSignalWait(SIG_INITADC, 500);
    if (ev.status == osEventTimeout)
    {
      msg::ERROR cmd{msg::error::SOUND_DAC};
      msg::send(appTaskHandle, msg::ERROR_NOTIFY, cmd);
      return;
    }
    if (msg::registerThread(4) != osOK)
    {
      return;
    }
    constexpr uint32_t BLOCK_SIZE_2 = satoh::BLOCK_SIZE * 2;
    constexpr uint32_t BLOCK_SIZE_4 = satoh::BLOCK_SIZE * 4;
    auto rxbuf = satoh::makeDmaMem<int32_t>(BLOCK_SIZE_4);    // 音声信号受信バッファ配列 Lch前半 Lch後半 Rch前半 Rch後半
    auto txbuf = satoh::makeDmaMem<int32_t>(BLOCK_SIZE_4);    // 音声信号送信バッファ配列
    auto left = satoh::makeDmaMem<float>(satoh::BLOCK_SIZE);  // RAM節約のためDMAメモリを使う
    auto right = satoh::makeDmaMem<float>(satoh::BLOCK_SIZE); // RAM節約のためDMAメモリを使う
    if (!rxbuf || !txbuf || !left || !right)
    {
      msg::ERROR e{msg::error::SOUND_MEM};
      msg::send(appTaskHandle, msg::ERROR_NOTIFY, e);
      return;
    }
    extern SAI_HandleTypeDef hsai_BlockA1;
    extern SAI_HandleTypeDef hsai_BlockB1;
    HAL_SAI_Transmit_DMA(&hsai_BlockB1, reinterpret_cast<uint8_t *>(txbuf.get()), BLOCK_SIZE_4);
    HAL_SAI_Receive_DMA(&hsai_BlockA1, reinterpret_cast<uint8_t *>(rxbuf.get()), BLOCK_SIZE_4);
    msg::SOUND_EFFECTOR effector{};
    fx::PopNoiseReductor pop(satoh::BLOCK_SIZE);
    for (;;)
    {
      auto res = msg::recv();
      auto *msg = res.msg();
      if (!msg)
      {
        continue;
      }
      switch (msg->type)
      {
      case msg::SOUND_DMA_HALF_NOTIFY:
        soundProc(effector, pop, rxbuf.get(), txbuf.get(), left.get(), right.get(), satoh::BLOCK_SIZE);
        break;
      case msg::SOUND_DMA_CPLT_NOTIFY:
        soundProc(effector, pop, rxbuf.get() + BLOCK_SIZE_2, txbuf.get() + BLOCK_SIZE_2, left.get(), right.get(), satoh::BLOCK_SIZE);
        break;
      case msg::SOUND_CHANGE_EFFECTOR_REQ:
        effector = *msg->get<msg::SOUND_EFFECTOR>();
        pop.init();
        break;
      }
    }
  }
  /// @brief DMA半分受信コールバック
  /// @param [in] hsai SAI
  void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) { msg::send(soundTaskHandle, msg::SOUND_DMA_HALF_NOTIFY); }
  /// @brief DMA全部受信コールバック
  /// @param [in] hsai SAI
  void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) { msg::send(soundTaskHandle, msg::SOUND_DMA_CPLT_NOTIFY); }
}
