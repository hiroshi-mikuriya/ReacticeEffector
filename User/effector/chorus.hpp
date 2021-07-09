/// @file      effector/chorus.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_calc.hpp"
#include "lib_delay.hpp"
#include "lib_filter.hpp"
#include "lib_osc.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class Chorus;
}

/// @brief Sodiumから拝借したコーラス
class satoh::Chorus : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    MIX,       ///< ミックス
    FBACK,     ///< フィードバック
    RATE,      ///< 周期
    DEPTH,     ///< 深さ
    TONE,      ///< トーン
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  signalSw bypass;             ///< ポップノイズ対策
  sineWave sin1;
  delayBufF del1;
  hpf hpf1;
  lpf2nd lpf2nd1;
  lpf2nd lpf2nd2;
  float level_;
  float mix_;
  float fback_;
  float depth_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint32_t n)
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -20.0f, 20.0f); // LEVEL -20 ～ 20dB;
      break;
    case MIX:
      mix_ = mixPot(ui_[MIX].getValue(), -20.0f); // MIX;
      break;
    case FBACK:
      fback_ = ui_[FBACK].getValue() / 100.0f; // Feedback 0～99%;
      break;
    case RATE:
    {
      float rate = 0.02f * (105.0f - ui_[RATE].getValue()); // RATE 周期 2.1～0.1 秒;
      sin1.set(1.0f / rate);
      break;
    }
    case DEPTH:
      depth_ = 0.05f * ui_[DEPTH].getValue(); // Depth ±5ms;
      break;
    case TONE:
    {
      float tone = 800.0f * logPot(ui_[TONE].getValue(), 0.0f, 20.0f); // HI CUT FREQ 800 ～ 8000 Hz
      lpf2nd1.set(tone);
      lpf2nd2.set(tone);
      break;
    }
    }
  }

public:
  /// @brief コンストラクタ
  Chorus() //
      : ui_({
            EffectParameterF(1, 100, 1, "LEVEL"), //
            EffectParameterF(1, 100, 1, "MIX"),   //
            EffectParameterF(1, 99, 1, "F.BACK"), //
            EffectParameterF(1, 100, 1, "RATE"),  //
            EffectParameterF(1, 100, 1, "DEPTH"), //
            EffectParameterF(1, 100, 1, "TONE"),  //
        }),                                       //
        level_(0),                                //
        mix_(0),                                  //
        fback_(0),                                //
        depth_(0)                                 //
  {
    del1.set(20.0f);  // 最大ディレイタイム設定
    hpf1.set(100.0f); // ディレイ音のローカット設定
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
  }
  /// @brief デストラクタ
  virtual ~Chorus() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float dtime = 5.0f + depth_ * (1.0f + sin1.output()); // ディレイタイム 5～15ms
      float fx = del1.readLerp(dtime);                      // ディレイ音読込(線形補間)
      fx = lpf2nd1.process(fx);                             // ディレイ音のTONE(ハイカット)
      fx = lpf2nd2.process(fx);
      // ディレイ音と原音をディレイバッファに書込、原音はローカットして書込
      del1.write(fback_ * fx + hpf1.process(right[i]));
      fx = (1.0f - mix_) * right[i] + mix_ * fx; // MIX
      fx = 1.4f * level_ * fx;                   // LEVEL
      right[i] = bypass.process(right[i], fx, true);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "Chorus";
    strcpy(buf, name);
    return sizeof(name);
  }
  /// @brief パラメータ数を取得
  /// @return パラメータ数
  uint32_t getParamCount() const noexcept override { return COUNT; }
  /// @brief パラメータ取得
  /// @param[in] n 取得対象のパラメータ番号
  float getParam(uint32_t n) const noexcept override { return ui_[n].getValue(); }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] v 値
  void setParam(uint32_t n, float v) noexcept { ui_[n].setValue(v); }
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  void incrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].increment();
      convUiToFx(n);
    }
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  void decrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].decrement();
      convUiToFx(n);
    }
  }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] ratio 比率（0.0f 〜 1.0f）
  void setParamRatio(uint32_t n, float ratio) noexcept override
  {
    if (n < COUNT)
    {
      ui_[n].setValue(ratio);
      convUiToFx(n);
    }
  }
  /// @brief パラメータ名文字列取得
  /// @param[in] n パラメータ番号
  /// @param[out] buf 文字列格納先
  /// @return 文字列の長さ
  uint32_t getParamName(uint32_t n, char *buf) const noexcept override
  {
    if (n < COUNT)
    {
      return ui_[n].getName(buf);
    }
    return 0;
  }
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @param[out] buf 文字列格納先
  /// @return 文字列の長さ
  uint32_t getValueTxt(uint32_t n, char *buf) const noexcept override
  {
    switch (n)
    {
    case LEVEL:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[LEVEL].getValue())));
    case MIX:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[MIX].getValue())));
    case FBACK:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[FBACK].getValue())));
    case RATE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[RATE].getValue())));
    case DEPTH:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[DEPTH].getValue())));
    case TONE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[TONE].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x00, 0x10, 0x20}; }
};
