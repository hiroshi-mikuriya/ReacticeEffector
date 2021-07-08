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

/// @brief Sodiumから拝借したトレモロ
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

  EffectParameter<float> params_[COUNT]; ///< パラメータ
  signalSw bypass;                       ///< ポップノイズ対策
  sineWave sin1;
  delayBufF del1;
  hpf hpf1;
  lpf2nd lpf2nd1;
  lpf2nd lpf2nd2;
  float level_;
  float mix_;
  float fback_;
  float depth_;
  /// @brief レベル更新
  void updateLevel() noexcept
  {
    level_ = logPot(params_[LEVEL].getValue(), -20.0f, 20.0f); // LEVEL -20 ～ 20dB;
  }
  /// @brief ミックス更新
  void updateMix() noexcept
  {
    mix_ = mixPot(params_[MIX].getValue(), -20.0f); // MIX;
  }
  /// @brief F.BACK更新
  void updateFback() noexcept
  {
    fback_ = params_[FBACK].getValue() / 100.0f; // Feedback 0～99%;
  }
  /// @brief RATE更新
  void updateRate() noexcept
  {
    float rate = 0.02f * (105.0f - params_[RATE].getValue()); // RATE 周期 2.1～0.1 秒;
    sin1.set(1.0f / rate);
  }
  /// @brief DEPTH更新
  void updateDepth() noexcept
  {
    depth_ = 0.05f * params_[DEPTH].getValue(); // Depth ±5ms;
  }
  /// @brief TONE更新
  void updateTone() noexcept
  {
    float tone = 800.0f * logPot(params_[TONE].getValue(), 0.0f, 20.0f); // HI CUT FREQ 800 ～ 8000 Hz
    lpf2nd1.set(tone);
    lpf2nd2.set(tone);
  }

public:
  /// @brief コンストラクタ
  Chorus() //
      : params_({
            EffectParameter<float>(1, 100, 1, "LEVEL"), //
            EffectParameter<float>(1, 100, 1, "MIX"),   //
            EffectParameter<float>(1, 99, 1, "F.BACK"), //
            EffectParameter<float>(1, 100, 1, "RATE"),  //
            EffectParameter<float>(1, 100, 1, "DEPTH"), //
            EffectParameter<float>(1, 100, 1, "TONE"),  //
        }),                                             //
        level_(0),                                      //
        mix_(0),                                        //
        fback_(0),                                      //
        depth_(0)                                       //
  {
    del1.set(20.0f);  // 最大ディレイタイム設定
    hpf1.set(100.0f); // ディレイ音のローカット設定
    updateLevel();
    updateMix();
    updateFback();
    updateRate();
    updateDepth();
    updateTone();
  }
  /// @brief デストラクタ
  virtual ~Chorus() {}
  /// @brief エフェクト処理実行
  /// @param[inout] v 音声データ
  /// @param[in] size 音声データ数
  void effect(float *v, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float dtime = 5.0f + depth_ * (1.0f + sin1.output()); // ディレイタイム 5～15ms
      float fx = del1.readLerp(dtime);                      // ディレイ音読込(線形補間)
      fx = lpf2nd1.process(fx);                             // ディレイ音のTONE(ハイカット)
      fx = lpf2nd2.process(fx);
      // ディレイ音と原音をディレイバッファに書込、原音はローカットして書込
      del1.write(fback_ * fx + hpf1.process(v[i]));
      fx = (1.0f - mix_) * v[i] + mix_ * fx; // MIX
      fx = 1.4f * level_ * fx;               // LEVEL
      v[i] = bypass.process(v[i], fx, true);
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
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  void incrementParam(uint32_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      params_[LEVEL].increment();
      updateLevel();
      break;
    case MIX:
      params_[MIX].increment();
      updateMix();
      break;
    case FBACK:
      params_[FBACK].increment();
      updateFback();
      break;
    case RATE:
      params_[RATE].increment();
      updateRate();
      break;
    case DEPTH:
      params_[DEPTH].increment();
      updateDepth();
      break;
    case TONE:
      params_[TONE].increment();
      updateTone();
      break;
    }
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  void decrementParam(uint32_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      params_[LEVEL].decrement();
      updateLevel();
      break;
    case MIX:
      params_[MIX].decrement();
      updateMix();
      break;
    case FBACK:
      params_[FBACK].decrement();
      updateFback();
      break;
    case RATE:
      params_[RATE].decrement();
      updateRate();
      break;
    case DEPTH:
      params_[DEPTH].decrement();
      updateDepth();
      break;
    case TONE:
      params_[TONE].decrement();
      updateTone();
      break;
    }
  }
  /// @brief パラメータ設定
  /// @param[in] n 減算対象のパラメータ番号
  /// @param[in] ratio 比率（0.0f 〜 1.0f）
  virtual void setParam(uint32_t n, float ratio) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      params_[LEVEL].setValue(ratio);
      updateLevel();
      break;
    case MIX:
      params_[MIX].setValue(ratio);
      updateMix();
      break;
    case FBACK:
      params_[FBACK].setValue(ratio);
      updateFback();
      break;
    case RATE:
      params_[RATE].setValue(ratio);
      updateRate();
      break;
    case DEPTH:
      params_[DEPTH].setValue(ratio);
      updateDepth();
      break;
    case TONE:
      params_[TONE].setValue(ratio);
      updateTone();
      break;
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
      return params_[n].getName(buf);
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
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[LEVEL].getValue())));
    case MIX:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[MIX].getValue())));
    case FBACK:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[FBACK].getValue())));
    case RATE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[RATE].getValue())));
    case DEPTH:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[DEPTH].getValue())));
    case TONE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[TONE].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x00, 0x00, 0x20}; }
};
