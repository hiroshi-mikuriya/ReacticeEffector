/// @file      effector/lib/lib_float.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
template <typename T>
T fromFloat(float v);

template <typename T>
float toFloat(T v);
} // namespace satoh

/// @brief floatからの変換（int8_t版）
/// @param [in] v -1.0f 〜 1.0f
/// @return 変換値
template <>
inline int8_t satoh::fromFloat<int8_t>(float v)
{
  return static_cast<int8_t>(v * 127);
}

/// @brief floatへの変換（int8_t版）
/// @param [in] v 値
/// @return -1.0f 〜 1.0f
template <>
inline float satoh::toFloat<int8_t>(int8_t v)
{
  return v / 127.0f;
}

/// @brief floatからの変換（int16_t版）
/// @param [in] v -1.0f 〜 1.0f
/// @return 変換値
template <>
inline int16_t satoh::fromFloat<int16_t>(float v)
{
  return static_cast<int16_t>(v * 32767);
}

/// @brief floatへの変換（int16_t版）
/// @param [in] v 値
/// @return -1.0f 〜 1.0f
template <>
inline float satoh::toFloat<int16_t>(int16_t v)
{
  return v / 32767.0f;
}

/// @brief floatからの変換（float版）
/// @param [in] v -1.0f 〜 1.0f
/// @return 変換値
template <>
inline float satoh::fromFloat<float>(float v)
{
  return v;
}

/// @brief floatへの変換（float版）
/// @param [in] v 値
/// @return -1.0f 〜 1.0f
template <>
inline float satoh::toFloat<float>(float v)
{
  return v;
}
