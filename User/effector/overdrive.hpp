/// @file      effector/overdrive.hpp
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
class OverDrive;
}

/// @brief Sodiumから拝借したオーバードライブ
class satoh::OverDrive : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    GAIN,      ///< ゲイン
    TREBLE,    ///< トレブル
    BASS,      ///< ベース
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  signalSw bypass;             ///< ポップノイズ対策
  hpf hpfFixed;                ///< 出力ローカット
  hpf hpfBass;                 ///< 入力BASS調整
  lpf lpfFixed;                ///< 入力ハイカット
  lpf lpfTreble;               ///< 出力TREBLE調整
  float level_;                ///< レベル
  float gain_;                 ///< ゲイン

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint32_t n)
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -50.0f, 0.0f); // LEVEL -50～0 dB
      break;
    case GAIN:
      gain_ = logPot(ui_[GAIN].getValue(), 20.0f, 60.0f); // GAIN 20～60 dB
      break;
    case TREBLE:
    {
      float treble = 10000.0f * logPot(ui_[TREBLE].getValue(), -30.0f, 0.0f); // TREBLE LPF 320～10k Hz
      lpfTreble.set(treble);
      break;
    }
    case BASS:
    {
      float bass = 2000.0f * logPot(ui_[BASS].getValue(), 0.0f, -20.0f); // BASS HPF 200～2000 Hz
      hpfBass.set(bass);
      break;
    }
    }
  }

public:
  /// @brief コンストラクタ
  OverDrive() //
      : ui_({
            EffectParameterF(1, 100, 1, "LEVEL"),  //
            EffectParameterF(1, 100, 1, "GAIN"),   //
            EffectParameterF(1, 100, 1, "TREBLE"), //
            EffectParameterF(1, 100, 1, "BASS"),   //
        }),                                        //
        level_(0),                                 //
        gain_(0)                                   //
  {
    lpfFixed.set(4000.0f); // 入力ハイカット 固定値
    hpfFixed.set(30.0f);   // 出力ローカット 固定値
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
  }
  /// @brief デストラクタ
  virtual ~OverDrive() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      fx = hpfBass.process(fx);   // 入力ローカット BASS
      fx = lpfFixed.process(fx);  // 入力ハイカット 固定値
      fx = gain_ * fx;            // GAIN
      fx = atanf(fx + 0.5f);      // arctanによるクリッピング、非対称化
      fx = hpfFixed.process(fx);  // 出力ローカット 固定値 直流カット
      fx = lpfTreble.process(fx); // 出力ハイカット TREBLE
      fx = level_ * fx;           // LEVEL
      right[i] = bypass.process(right[i], fx, true);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "OverDrive";
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
    case TREBLE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[TREBLE].getValue())));
    case BASS:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[BASS].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x20, 0x20, 0x00}; }
};
