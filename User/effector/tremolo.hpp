/// @file      effector/tremolo.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib_calc.hpp"
#include "lib_osc.hpp"
#include <cstdio> // sprintf

namespace satoh
{
class Tremolo;
}

/// @brief Sodiumから拝借したトレモロ
class satoh::Tremolo : public satoh::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    RATE,      ///< 周期
    DEPTH,     ///< 深さ
    WAVE,      ///< 波形
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  signalSw bypass;             ///< ポップノイズ対策
  float level_;                ///< レベル
  float wave_;                 ///< 波形
  float depth_;                ///< 深さ
  triangleWave tri;            ///< 周期

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint32_t n)
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -20.0f, 20.0f); // LEVEL -20～20 dB
      break;
    case RATE:
    {
      float rate = 0.01f * (105.0f - ui_[RATE].getValue()); // RATE 周期 1.05～0.05 秒
      tri.set(1.0f / rate);                                 // 三角波 周波数設定
      break;
    }
    case DEPTH:
      depth_ = ui_[DEPTH].getValue() * 0.1f; // DEPTH -10～10 dB
      break;
    case WAVE:
      wave_ = logPot(ui_[WAVE].getValue(), 0.0f, 50.0f); // WAVE 三角波～矩形波変形
      break;
    }
  }

public:
  /// @brief コンストラクタ
  Tremolo() //
      : ui_({
            EffectParameterF(1, 100, 1, "LEVEL"), //
            EffectParameterF(1, 100, 1, "RATE"),  //
            EffectParameterF(1, 100, 1, "DEPTH"), //
            EffectParameterF(1, 100, 1, "WAVE"),  //
        }),                                       //
        level_(0),                                //
        wave_(0),                                 //
        depth_(0)                                 //
  {
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
  }
  /// @brief デストラクタ
  virtual ~Tremolo() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      float gain = 2.0f * tri.output() - 1.0f; // LFO -1～1 三角波
      gain = wave_ * gain;                     // 三角波を増幅
      satoh::compress(-1.0f, gain, 1.0f);      // クリッピング（矩形波に近い形へ）
      gain = depth_ * gain;                    // DEPTH -10～10 dB
      fx = dbToGain(gain) * fx;                // 音量を揺らす
      fx = level_ * fx;                        // LEVEL
      right[i] = bypass.process(right[i], fx, true);
    }
  }
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "Tremolo";
    strcpy(buf, name);
    return sizeof(name);
  }
  /// @brief パラメータ数を取得
  /// @return パラメータ数
  uint32_t getParamCount() const noexcept override { return COUNT; }
  /// @brief パラメータ取得
  /// @param[in] n 取得対象のパラメータ番号
  /// @return パラメータ
  float getParam(uint32_t n) const noexcept override { return ui_[n].getValue(); }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] v 値
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  bool setParam(uint32_t n, float v) noexcept { return ui_[n].setValue(v); }
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  /// @retval true 加算した
  /// @retval false 最大値に到達しているため加算しなかった
  bool incrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      if (ui_[n].increment())
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  /// @retval true 減算した
  /// @retval false 最小値に到達しているため減算しなかった
  bool decrementParam(uint32_t n) noexcept override
  {
    if (n < COUNT)
    {
      if (ui_[n].decrement())
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ比率設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] ratio 比率（最小値 0.0f 〜 1.0f 最大値）
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  bool setParamRatio(uint32_t n, float ratio) noexcept override
  {
    if (n < COUNT)
    {
      if (ui_[n].setValueRatio(ratio))
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
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
    case RATE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[RATE].getValue())));
    case WAVE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[WAVE].getValue())));
    case DEPTH:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[DEPTH].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x00, 0x20, 0x00}; }
};
