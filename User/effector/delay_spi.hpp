/// @file      effector/delay_spi.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/alloc.hpp"
#include "delay_base.hpp"
#include "lib/lib_delay_spi.hpp"

namespace satoh
{
namespace fx
{
class DelaySpi;
}
} // namespace satoh

/// @brief SPI SRAMをバッファに使用するディレイ
class satoh::fx::DelaySpi : public satoh::fx::DelayBase
{
  delaySpiBuf<int16_t> delayBuf_;
  UniquePtr<float> rbuf_;
  UniquePtr<float> wbuf_;

  /// @brief DTIMEを更新する
  void updateDtime() noexcept override { delayBuf_.setInterval(ui_[DTIME].getValue()); }

public:
  /// @brief コンストラクタ
  /// @param[in] spi SPI通信オブジェクト
  explicit DelaySpi(SpiMaster *spi)                                     //
      : DelayBase(DELAY_SPI, "DelaySpi", "DLS", RGB{0x20, 0x00, 0x20}), //
        delayBuf_(spi, BLOCK_SIZE, ui_[DTIME].getMax()),                //
        rbuf_(allocArray<float>(BLOCK_SIZE)),                           //
        wbuf_(allocArray<float>(BLOCK_SIZE))                            //
  {
    if (*this)
    {
      init(ui_, COUNT);
      memset(rbuf_.get(), 0, BLOCK_SIZE * sizeof(float));
      memset(wbuf_.get(), 0, BLOCK_SIZE * sizeof(float));
    }
  }
  /// @brief デストラクタ
  virtual ~DelaySpi() {}
  /// @brief エフェクターセットアップ成功・失敗
  /// @retval true 成功
  /// @retval false 失敗
  explicit operator bool() const noexcept override { return delayBuf_ && rbuf_ && wbuf_; }
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    float *r = rbuf_.get();
    float *w = wbuf_.get();
    delayBuf_.read(r);
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = r[i];
      fx = lpf2ndTone_.process(fx);
      w[i] = fback_ * fx + right[i];
      right[i] += fx * elevel_;
    }
    delayBuf_.write(w);
  }
};
