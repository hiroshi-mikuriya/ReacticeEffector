/// @file      effector/booster.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_filter.hpp"
#include <cmath>  // powf
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class Booster;
}
} // namespace satoh

/// @brief Sodiumから拝借したBooster
class satoh::fx::Booster : public satoh::fx::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    HIGH,      ///< ハイ
    LOW,       ///< ロー
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  float gain_;
  hpf hpf_;
  lpf lpf_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      gain_ = std::pow(10.0f, (2.0f * ui_[LEVEL].getValue() - 10.0f) / 20.0f); // 音量調整 dB計算
      break;
    case HIGH:
    {
      float hpfFreq = 1000.0f * std::pow(0.794f, ui_[HIGH].getValue()); // ハイパスフィルタ周波数計算
      hpf_.set(hpfFreq);
      break;
    }
    case LOW:
    {
      float lpfFreq = 500.0f * std::pow(1.259f, ui_[LOW].getValue()); // ローパスフィルタ周波数計算
      lpf_.set(lpfFreq);
      break;
    }
    }
  }
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override
  {
    switch (n)
    {
    case LEVEL:
    case HIGH:
    case LOW:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Booster()
      : EffectorBase(BOOSTER, "Booster", "BT", RGB{0x20, 0x00, 0x00}), //
        ui_({
            EffectParameterF(0, 10, 1, "LEVEL"), //
            EffectParameterF(0, 10, 1, "HIGH"),  //
            EffectParameterF(0, 10, 1, "LOW"),   //
        }),                                      //
        gain_(0)                                 //
  {
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~Booster() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      fx = lpf_.process(fx); // ローパスフィルタ
      fx = hpf_.process(fx); // ハイパスフィルタ
      fx = fx * gain_;       // 音量調整
      satoh::fx::compress(0, fx, 0.99f);
      right[i] = fx;
    }
  }
};
