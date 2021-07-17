#pragma once

#include "table_dbToGain.h"
#include <cmath>

inline float gainToDb(float x) // 使用範囲0.00001(-100dB)～1(0dB) 最大誤差0.016dB
{
  x = sqrtf(sqrtf(sqrtf(sqrtf(sqrtf(x)))));
  return -559.57399f + 995.83468f * x - 591.85129f * x * x + 155.60596f * x * x * x;
}

inline float dbToGain(float x) // 使用範囲±128dB 最大誤差0.015dB
{
  return dbToGainTable[(uint8_t)(x + 128.0f)] +
         (dbToGainTable[(uint8_t)(x + 128.0f) + 1] - dbToGainTable[(uint8_t)(x + 128.0f)]) * ((x + 128.0f) - (float)((uint8_t)(x + 128.0f)));
}

inline float logPot(uint16_t pot, float dBmin, float dBmax)
{
  // パラメータの値0～100を最小dB～最大dB倍率へ割り当てる
  float p = (dBmax - dBmin) * (float)pot * 0.01f + dBmin;
  return dbToGain(p); // dBから倍率へ変換
}

inline float mixPot(uint16_t pot, float dBmin)
{
  // パラメータの値0～100をMIX倍率へ割り当てる
  // dBminはdB変化の初期値で-6未満の値
  float a = (-6.0206f - dBmin) * 0.02f; // dB変化の傾きを計算
  if (pot == 0)
    return 0.0f;
  else if (pot < 50)
    return dbToGain((float)pot * a + dBmin); // dBmin ～ -6dB
  else if (pot < 100)
    return 1.0f - dbToGain((float)(100 - pot) * a + dBmin);
  else
    return 1.0f;
}

/* ポップノイズ対策のため、0.01ずつ音量変更しスイッチ操作する（エフェクトバイパス等）--------------*/
class signalSw
{
  uint8_t count_ = 0;

public:
  /// @brief エフェクト前後の音をミックスして返す
  /// @param[in] x エフェクト前
  /// @param[in] fx エフェクト後
  /// @param[in] active エフェクトON/OFF
  float process(float x, float fx, bool active)
  {
    if (active) // エフェクトON
    {
      if (count_ < 100) // バイパス音量ダウン
      {
        count_++;
        return (1.0f - count_ * 0.01f) * x; // (count: 1～100)
      }
      if (count_ < 200) // エフェクト音量アップ
      {
        count_++;
        return (count_ * 0.01f - 1.0f) * fx; // (count: 101～200)
      }
      return fx;
    }
    else // エフェクトOFF
    {
      if (count_ > 100) // エフェクト音量ダウン
      {
        count_--;
        return (count_ * 0.01f - 1.0f) * fx; // (count: 199～100)
      }
      if (count_ > 0) // バイパス音量アップ
      {
        count_--;
        return (1.0f - count_ * 0.01f) * x; // (count: 99～0)
      }
      return x;
    }
  }
};
