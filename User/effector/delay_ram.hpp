/// @file      effector/delay_ram.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "delay_base.hpp"
#include "lib/lib_delay.hpp"

namespace satoh
{
namespace fx
{
class DelayRam;
}
} // namespace satoh

/// @brief 内部RAMをバッファとして使用するディレイ
class satoh::fx::DelayRam : public satoh::fx::DelayBase
{
  delayBuf<int16_t> delayBuf_;
  /// @brief DTIMEを更新する
  void updateDtime() noexcept override { delayBuf_.setInterval(ui_[DTIME].getValue()); }

public:
  /// @brief コンストラクタ
  DelayRam()                                                        //
      : DelayBase(DELAY_RAM, "Delay", "DL", RGB{0x20, 0x00, 0x20}), //
        delayBuf_(ui_[DTIME].getMax())                              //
  {
    if (*this)
    {
      init(ui_, COUNT);
    }
  }
  /// @brief デストラクタ
  virtual ~DelayRam() {}
  /// @brief エフェクターセットアップ成功・失敗
  /// @retval true 成功
  /// @retval false 失敗
  explicit operator bool() const noexcept override { return static_cast<bool>(delayBuf_); }
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = delayBuf_.read();
      fx = lpf2ndTone_.process(fx);
      delayBuf_.write(fback_ * fx + right[i]);
      right[i] += fx * elevel_;
    }
  }
};
