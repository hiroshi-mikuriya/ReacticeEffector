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

  EffectParameter<float> params_[COUNT]; ///< パラメータ
  signalSw bypass;                       ///< ポップノイズ対策
  hpf hpfFixed;                          ///< 出力ローカット
  hpf hpfBass;                           ///< 入力BASS調整
  lpf lpfFixed;                          ///< 入力ハイカット
  lpf lpfTreble;                         ///< 出力TREBLE調整
  float level_;                          ///< レベル
  float gain_;                           ///< ゲイン
  /// @brief レベル更新
  void updateLevel() noexcept
  {
    level_ = logPot(params_[LEVEL].getValue(), -50.0f, 0.0f); // LEVEL -50～0 dB
  }
  /// @brief ゲイン更新
  void updateGain() noexcept
  {
    gain_ = logPot(params_[GAIN].getValue(), 20.0f, 60.0f); // GAIN 20～60 dB
  }
  /// @brief トラブル更新
  void updateTreble() noexcept
  {
    float treble = 10000.0f * logPot(params_[TREBLE].getValue(), -30.0f, 0.0f); // TREBLE LPF 320～10k Hz
    lpfTreble.set(treble);
  }
  /// @brief ベース更新
  void updateBass() noexcept
  {
    float bass = 2000.0f * logPot(params_[BASS].getValue(), 0.0f, -20.0f); // BASS HPF 200～2000 Hz
    hpfBass.set(bass);
  }

public:
  /// @brief コンストラクタ
  OverDrive() //
      : params_({
            EffectParameter<float>(1, 100, 1, "LEVEL"),  //
            EffectParameter<float>(1, 100, 1, "GAIN"),   //
            EffectParameter<float>(1, 100, 1, "TREBLE"), //
            EffectParameter<float>(1, 100, 1, "BASS"),   //
        }),                                              //
        level_(0),                                       //
        gain_(0)                                         //
  {
    lpfFixed.set(4000.0f); // 入力ハイカット 固定値
    hpfFixed.set(30.0f);   // 出力ローカット 固定値
    updateLevel();
    updateGain();
    updateTreble();
    updateBass();
  }
  /// @brief デストラクタ
  virtual ~OverDrive() {}
  /// @brief エフェクト処理実行
  /// @param[inout] v 音声データ
  /// @param[in] size 音声データ数
  void effect(float *v, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = v[i];
      fx = hpfBass.process(fx);   // 入力ローカット BASS
      fx = lpfFixed.process(fx);  // 入力ハイカット 固定値
      fx = gain_ * fx;            // GAIN
      fx = atanf(fx + 0.5f);      // arctanによるクリッピング、非対称化
      fx = hpfFixed.process(fx);  // 出力ローカット 固定値 直流カット
      fx = lpfTreble.process(fx); // 出力ハイカット TREBLE
      fx = level_ * fx;           // LEVEL
      v[i] = bypass.process(v[i], fx, true);
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
    case GAIN:
      params_[GAIN].increment();
      updateGain();
      break;
    case TREBLE:
      params_[TREBLE].increment();
      updateTreble();
      break;
    case BASS:
      params_[BASS].increment();
      updateBass();
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
    case GAIN:
      params_[GAIN].decrement();
      updateGain();
      break;
    case TREBLE:
      params_[TREBLE].decrement();
      updateTreble();
      break;
    case BASS:
      params_[BASS].decrement();
      updateBass();
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
    case GAIN:
      params_[GAIN].setValue(ratio);
      updateGain();
      break;
    case TREBLE:
      params_[TREBLE].setValue(ratio);
      updateTreble();
      break;
    case BASS:
      params_[BASS].setValue(ratio);
      updateBass();
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
    case GAIN:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[GAIN].getValue())));
    case TREBLE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[TREBLE].getValue())));
    case BASS:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(params_[BASS].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x20, 0x20, 0x00}; }
};
