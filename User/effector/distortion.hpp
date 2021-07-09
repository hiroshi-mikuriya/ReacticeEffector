/// @file      effector/distortion.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_calc.hpp"
#include "lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class Distortion;
}

/// @brief Sodiumから拝借したディストーション
class satoh::Distortion : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    GAIN,      ///< ゲイン
    TONE,      ///< トーン
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  signalSw bypass;             ///< ポップノイズ対策
  hpf hpf1;                    ///< ローカット1
  hpf hpf2;                    ///< ローカット2
  hpf hpfTone;                 ///< ローカットトーン調整用
  lpf lpf1;                    ///< ハイカット1
  lpf lpf2;                    ///< ハイカット2
  lpf lpfTone;                 ///< ハイカットトーン調整用
  float level_;                ///< レベル
  float gain_;                 ///< ゲイン
  float tone_;                 ///< トーン

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint32_t n)
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -50.0f, 0.0f); // LEVEL -50...0 dB
      break;
    case GAIN:
      gain_ = logPot(ui_[GAIN].getValue(), 5.0f, 45.0f); // GAIN 5...+45 dB
      break;
    case TONE:
      tone_ = mixPot(ui_[TONE].getValue(), -20.0f); // TONE 0～1 LPF側とHPF側をミックス
      break;
    }
  }

public:
  /// @brief コンストラクタ
  Distortion() //
      : ui_({
            EffectParameterF(1, 100, 1, "LEVEL"), //
            EffectParameterF(1, 100, 1, "GAIN"),  //
            EffectParameterF(1, 100, 1, "TONE"),  //
        }),                                       //
        level_(0),                                //
        gain_(0)                                  //
  {
    lpf1.set(5000.0f);    // ハイカット1 固定値
    lpf2.set(4000.0f);    // ハイカット2 固定値
    lpfTone.set(240.0f);  // TONE用ハイカット 固定値
    hpf1.set(40.0f);      // ローカット1 固定値
    hpf2.set(30.0f);      // ローカット2 固定値
    hpfTone.set(1000.0f); // TONE用ローカット 固定値
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
  }
  /// @brief デストラクタ
  virtual ~Distortion() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      fx = hpf1.process(fx); // ローカット1
      fx = lpf1.process(fx); // ハイカット1
      fx = 10.0f * fx;       // 1段目固定ゲイン
      if (fx < -0.5f)
      {
        fx = -0.25f; // 2次関数による波形の非対称変形
      }
      else
      {
        fx = fx * fx + fx;
      }
      fx = hpf2.process(fx);                       // ローカット2 直流カット
      fx = lpf2.process(fx);                       // ハイカット2
      fx = gain_ * fx;                             // GAIN
      fx = tanhf(fx);                              // tanhによる対称クリッピング
      fx = tone_ * hpfTone.process(fx)             // TONE
           + (1.0f - tone_) * lpfTone.process(fx); // LPF側とHPF側をミックス
      fx = level_ * fx;                            // LEVEL
      right[i] = bypass.process(right[i], fx, true);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "Distortion";
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
    case GAIN:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[GAIN].getValue())));
    case TONE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[TONE].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x20, 0x00, 0x00}; }
};
