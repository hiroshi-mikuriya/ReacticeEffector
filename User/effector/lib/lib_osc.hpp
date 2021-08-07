#pragma once

#include "constant.h"
#include "lib_calc.hpp"

namespace satoh
{
class SawWave;
class SinWave;
class TriangleWave;
} // namespace satoh

/// @brief のこぎり波
class satoh::SawWave
{
  float phase_ = 0.0f; ///< 位相
  float freq_ = 0.1f;  ///< 周波数

public:
  /// @brief 周波数設定 @param[in] freq 周波数
  void set(float freq) noexcept { freq_ = freq; }
  /// @brief 周波数、位相（0～1）設定 @param[in] freq 周波数 @param[in] phase 位相
  void set(float freq, float phase) noexcept
  {
    freq_ = freq;
    phase_ = phase;
  }
  /// @brief のこぎり波出力 0～1 @return 音声波形
  float output() noexcept
  {
    float ph = phase_ + freq_ / satoh::SAMPLING_FREQ;
    if (1 < ph)
    {
      ph -= 1;
    }
    return phase_ = ph; // 次回計算のために出力値を保存
  }
};

/// @brief 正弦波
class satoh::SinWave
{
  SawWave saw;

public:
  /// @brief 周波数設定 @param[in] freq 周波数
  void set(float freq) noexcept { saw.set(freq); }
  /// @brief 周波数、位相（0～1）設定 @param[in] freq 周波数 @param[in] phase 位相
  void set(float freq, float phase) noexcept { saw.set(freq, phase); }
  /// @brief 正弦波出力 -1～1 @return 音声波形
  float output() noexcept { return std::sin(2 * satoh::PI * saw.output()); }
};

/// @brief 三角波
class satoh::TriangleWave
{
  SawWave saw;

public:
  /// @brief 周波数設定 @param[in] freq 周波数
  void set(float freq) noexcept { saw.set(freq); }
  /// @brief 周波数、位相（0～1）設定 @param[in] freq 周波数 @param[in] phase 位相
  void set(float freq, float phase) noexcept { saw.set(freq, phase); }
  /// @brief 三角波出力 0～1 @return 音声波形
  float output() noexcept
  {
    float y = 2.0f * saw.output();
    return y < 1.0f ? y : 2.0f - y;
  }
};
