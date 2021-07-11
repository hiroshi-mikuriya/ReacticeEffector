/// @file      effector/phaser.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_filter.hpp"
#include "lib_osc.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class Phaser;
}

/// @brief Sodiumから拝借したフェイザー
class satoh::Phaser : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    RATE,      ///< 周期
    STAGE,     ///< ステージ
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  signalSw bypass;             ///< ポップノイズ対策
  triangleWave tri;
  apf apfx[12];
  float level_; ///< レベル
  float stage_; ///< ステージ

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -20.0f, 20.0f); // LEVEL -20～20 dB
      break;
    case RATE:
    {
      float rate = 0.02f * (105.0f - ui_[RATE].getValue()); // RATE 周期 2.1～0.1 秒
      tri.set(1.0f / rate);                                 // 三角波 周波数設定
      break;
    }
    case STAGE:
      stage_ = 0.1f + ui_[STAGE].getValue() * 2.0f; // STAGE 2～12 後で整数へ変換
      break;
    }
  }
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列の長さ
  const char *getValueTxtImpl(uint8_t n) const noexcept override
  {
    switch (n)
    {
    case LEVEL:
    case RATE:
    case STAGE:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Phaser()                                                  //
      : EffectorBase("Phaser", "PH", RGB{0x8, 0x20, 0x00}), //
        ui_({
            EffectParameterF(0, 100, 1, "LEVEL"), //
            EffectParameterF(0, 100, 1, "RATE"),  //
            EffectParameterF(1, 6, 1, "STAGE"),   //
        })                                        //
  {
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~Phaser() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      float lfo = 20.0f * tri.output();    // LFO 0～20 三角波
      float freq = 200.0f * dbToGain(lfo); // APF周波数 200～2000Hz 指数的変化
      for (uint8_t j = 0; j < stage_; j++) // 段数分APF繰り返し
      {
        apfx[j].set(freq);        // APF周波数設定
        fx = apfx[j].process(fx); // APF実行
      }
      fx = 0.7f * (right[i] + fx); // 原音ミックス、音量調整
      fx = level_ * fx;            // LEVEL
      right[i] = bypass.process(right[i], fx, true);
    }
  }
};
