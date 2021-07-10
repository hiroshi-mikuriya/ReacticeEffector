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
  signalSw bypass;             ///< ポップノイズ対策
  triangleWave tri;
  apf apfx[12];
  float level_; ///< レベル
  float stage_; ///< ステージ

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
      float rate = 0.02f * (105.0f - ui_[RATE].getValue()); // RATE 周期 2.1～0.1 秒
      tri.set(1.0f / rate);                                 // 三角波 周波数設定
      break;
    }
    case STAGE:
      stage_ = 0.1f + ui_[STAGE].getValue() * 2.0f; // STAGE 2～12 後で整数へ変換
      break;
    }
  }

public:
  /// @brief コンストラクタ
  Phaser() //
      : ui_({
            EffectParameterF(0, 100, 1, "LEVEL"), //
            EffectParameterF(0, 100, 1, "RATE"),  //
            EffectParameterF(1, 6, 1, "STAGE"),   //
        })                                        //
  {
    for (uint32_t n = 0; n < COUNT; ++n)
    {
      convUiToFx(n);
    }
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
  /// @brief エフェクト名を取得
  /// @param[out] buf 文字列格納先
  /// @return 文字数
  uint32_t getName(char *buf) const noexcept override
  {
    const char name[] = "Phaser";
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
    case STAGE:
      return static_cast<uint32_t>(sprintf(buf, "%d", static_cast<int>(ui_[STAGE].getValue())));
    default:
      return 0;
    }
  }
  /// @brief LED色を取得
  /// @return LED色
  RGB getColor() const noexcept override { return RGB{0x8, 0x20, 0x00}; }
};
