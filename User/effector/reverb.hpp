/// @file      effector/reverb.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_calc.hpp"
#include "lib_delay.hpp"
#include "lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class Reverb;
}

/// @brief エフェクタークラスの実装参考用テンプレート
class satoh::Reverb : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    MIX,       ///< ミックス
    FBACK,     ///< フィードバック
    HICUT,     ///< ハイカット
    LOCUT,     ///< ローカット
    HIDUMP,    ///< ハイダンプ
    COUNT,     ///< パラメータ総数
  };

  // ディレイタイム配列
  const float dt[10] = {43.5337, 25.796, 19.392, 16.364, 7.645, 4.2546, 58.6435, 69.4325, 74.5234, 86.1244};

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  signalSw bypassIn;
  signalSw bypassOutL;
  signalSw bypassOutR;
  delayBufF del[10];
  lpf lpfIn;
  lpf lpfFB[4];
  hpf hpfOutL;
  hpf hpfOutR;
  float level_;
  float mix_;
  float fback_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint32_t n)
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -20.0f, 20.0f); // LEVEL -20 ～ +20dB
      break;
    case MIX:
      mix_ = mixPot(ui_[MIX].getValue(), -20.0f); // MIX
      break;
    case FBACK:
      fback_ = ui_[FBACK].getValue() / 200.0f; // Feedback 0～0.495
      break;
    case HICUT:
    {
      float hicut = 600.0f * logPot(ui_[HICUT].getValue(), 20.0f, 0.0f); // HI CUT FREQ 600 ~ 6000 Hz
      lpfIn.set(hicut);
      break;
    }
    case LOCUT:
    {
      float locut = 100.0f * logPot(ui_[LOCUT].getValue(), 0.0f, 20.0f); // LOW CUT FREQ 100 ~ 1000 Hz
      hpfOutL.set(locut);
      hpfOutR.set(locut);
      break;
    }
    case HIDUMP:
    {
      float hidump = 600.0f * logPot(ui_[HIDUMP].getValue(), 20.0f, 0.0f); // Feedback HI CUT FREQ 600 ~ 6000 Hz
      lpfFB[0].set(hidump);
      lpfFB[1].set(hidump);
      lpfFB[2].set(hidump);
      lpfFB[3].set(hidump);
      break;
    }
    }
  }

public:
  /// @brief コンストラクタ
  Reverb() //
      : ui_({
            EffectParameterF(0, 100, 1, "LEVEL"),  //
            EffectParameterF(0, 100, 1, "MIX"),    //
            EffectParameterF(0, 99, 1, "F.BACK"),  //
            EffectParameterF(0, 100, 1, "HiCUT"),  //
            EffectParameterF(0, 100, 1, "LoCUT"),  //
            EffectParameterF(0, 100, 1, "HiDUMP"), //
        }),                                        //
        level_(0),                                 //
        mix_(0),                                   //
        fback_(0)                                  //
  {
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
    for (int i = 0; i < 10; i++)
    {
      del[i].set(dt[i]); // ディレイタイム設定
    }
  }
  /// @brief デストラクタ
  virtual ~Reverb() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fxR = bypassIn.process(0.0f, right[i], true);
      fxR = 0.25f * lpfIn.process(fxR);

      // Early Reflection

      del[0].write(fxR);
      float ap = fxR + del[0].readLerp(dt[0]);
      float am = fxR - del[0].readLerp(dt[0]);
      del[1].write(am);
      float bp = ap + del[1].readLerp(dt[1]);
      float bm = ap - del[1].readLerp(dt[1]);
      del[2].write(bm);
      float cp = bp + del[2].readLerp(dt[2]);
      float cm = bp - del[2].readLerp(dt[2]);
      del[3].write(cm);
      float dp = cp + del[3].readLerp(dt[3]);
      float dm = cp - del[3].readLerp(dt[3]);
      del[4].write(dm);
      float ep = dp + del[4].readLerp(dt[4]);
      float em = dp - del[4].readLerp(dt[4]);
      del[5].write(em);

      // Late Reflection & High Freq Dumping

      float hd = del[6].readLerp(dt[6]);
      hd = lpfFB[0].process(hd);

      float id = del[7].readLerp(dt[7]);
      id = lpfFB[1].process(id);

      float jd = del[8].readLerp(dt[8]);
      jd = lpfFB[2].process(jd);

      float kd = del[9].readLerp(dt[9]);
      kd = lpfFB[3].process(kd);

      float outR = ep + hd * fback_;
      float outL = del[5].readLerp(dt[5]) + id * fback_;

      float fp = outL + outR;
      float fm = outL - outR;
      float gp = jd * fback_ + kd * fback_;
      float gm = jd * fback_ - kd * fback_;
      del[6].write(fp + gp);
      del[7].write(fm + gm);
      del[8].write(fp - gp);
      del[9].write(fm - gm);

      fxR = (1.0f - mix_) * right[i] + mix_ * hpfOutR.process(outR);
      float fxL = (1.0f - mix_) * left[i] + mix_ * hpfOutL.process(outL);

      left[i] = bypassOutL.process(left[i], level_ * fxL, true);
      right[i] = bypassOutR.process(right[i], level_ * fxR, true);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "FX Template";
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
    if (n < COUNT)
    {
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[n].getValue())));
    }
    return 0;
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x20, 0x20, 0x20}; }
};
