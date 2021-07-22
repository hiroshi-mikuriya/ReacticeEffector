#pragma once

#include "constant.h"
#include <memory>

namespace satoh
{
template <typename T>
class delayBuf;

template <typename T>
T fromFloat(float v);

template <typename T>
float toFloat(T v);
} // namespace satoh

/// @brief floatからの変換（int8_t版）
/// @param[in] v -0.1f 〜 0.1f
/// @return 変換値
template <>
inline int8_t satoh::fromFloat<int8_t>(float v)
{
  return static_cast<int8_t>(v * 127);
}

/// @brief floatへの変換（int8_t版）
/// @param[in] v 値
/// @return -0.1f 〜 0.1f
template <>
inline float satoh::toFloat<int8_t>(int8_t v)
{
  return v / 127.0f;
}

/// @brief floatからの変換（int16_t版）
/// @param[in] v -0.1f 〜 0.1f
/// @return 変換値
template <>
inline int16_t satoh::fromFloat<int16_t>(float v)
{
  return static_cast<int16_t>(v * 32767);
}

/// @brief floatへの変換（int16_t版）
/// @param[in] v 値
/// @return -0.1f 〜 0.1f
template <>
inline float satoh::toFloat<int16_t>(int16_t v)
{
  return v / 32767.0f;
}

/// @brief floatからの変換（float版）
/// @param[in] v -0.1f 〜 0.1f
/// @return 変換値
template <>
inline float satoh::fromFloat<float>(float v)
{
  return v;
}

/// @brief floatへの変換（float版）
/// @param[in] v 値
/// @return -0.1f 〜 0.1f
template <>
inline float satoh::toFloat<float>(float v)
{
  return v;
}

/// @brief ディレイバッファ
/// @tparam T 保持するデータ型
template <typename T>
class satoh::delayBuf
{
  /// @brief デフォルトコンストラクタ削除
  delayBuf() = delete;
  /// @brief コピーコンストラクタ削除
  delayBuf(delayBuf const &) = delete;
  /// @brief コピー演算子削除
  delayBuf &operator=(delayBuf const &) = delete;

  /// @brief 書込位置と読出位置の間隔を計算
  static constexpr float getInterval(float time) { return 0.001f * time * satoh::SAMPLING_FREQ; }
  /// @brief バッファサイズ取得
  /// @param[in] time 最大保持時間（ミリ秒）
  /// @return バッファサイズ
  static constexpr uint32_t getBufferSize(float time) { return 1 + static_cast<uint32_t>(getInterval(time)); }

  uint32_t maxDelaySample_; // 最大ディレイタイム サンプル数
  std::unique_ptr<T> buf_;  // ディレイバッファ配列
  uint32_t wpos_;           // write position 書き込み位置

  /// @brief float値を変換して保存する
  /// @param[in] pos 格納先のインデックス
  /// @param[in] v 値
  void setFloat(uint32_t pos, float v) noexcept { buf_.get()[pos] = fromFloat<T>(v); }
  /// @brief 値を読み出して、floatへ変換して取得する
  /// @param[in] pos 読み出し先のインデックス
  /// @return float値
  float getFloat(uint32_t pos) const noexcept { return toFloat<T>(buf_.get()[pos]); }

public:
  /// @brief コンストラクタ
  /// @param[in] maxTime 最大保持時間（ミリ秒）
  explicit delayBuf(float maxTime) noexcept      //
      : maxDelaySample_(getBufferSize(maxTime)), //
        buf_(new T[maxDelaySample_]),            //
        wpos_(0)                                 //
  {
    memset(buf_.get(), 0, maxDelaySample_ * sizeof(T));
  }
  /// @brief moveコンストラクタ
  /// @param[in] that 移動元
  explicit delayBuf(delayBuf &&that) noexcept  //
      : maxDelaySample_(that.maxDelaySample_), //
        buf_(std::move(that.buf_)),            //
        wpos_(that.wpos_)                      //
  {
  }
  /// @brief move演算子
  /// @param[in] that 移動元
  /// @return 自身の参照
  delayBuf &operator=(delayBuf &&that) noexcept
  {
    if (this != &that)
    {
      maxDelaySample_ = that.maxDelaySample_;
      buf_ = std::move(that.buf_);
      wpos_ = that.wpos_;
    }
    return *this;
  }
  /// @brief デストラクタ
  virtual ~delayBuf() {}
  /// @brief バッファ配列書き込み、書込位置を進める
  /// @param[in] v 書き込むデータ
  void write(float v) noexcept
  {
    setFloat(wpos_, v);
    wpos_ = (wpos_ + 1) % maxDelaySample_;
  }
  /// @brief 通常のサンプル単位での読み出し
  /// @param[in] delayTime ディレイ時間 ms
  float read(float delayTime) const noexcept
  {
    uint32_t interval = static_cast<uint32_t>(getInterval(delayTime));
    interval = std::min(interval, maxDelaySample_);
    uint32_t rpos = interval <= wpos_                        //
                        ? wpos_ - interval                   //
                        : wpos_ + maxDelaySample_ - interval //
        ;
    return getFloat(rpos);
  }
  /// @brief 線形補間して読み出し（コーラス等に利用）
  /// @param[in] delayTime ディレイ時間 ms
  float readLerp(float delayTime) const noexcept
  {
    float intervalF = std::min(getInterval(delayTime), static_cast<float>(maxDelaySample_));
    float rposF = intervalF <= wpos_                        //
                      ? wpos_ - intervalF                   //
                      : wpos_ + maxDelaySample_ - intervalF //
        ;
    uint32_t rpos0 = static_cast<uint32_t>(rposF);
    uint32_t rpos1 = (rpos0 + 1) % maxDelaySample_;
    float t = rposF - rpos0;
    return (1 - t) * getFloat(rpos0) + t * getFloat(rpos1);
  }
  /// @brief 固定時間（最大ディレイタイム）で読み出し
  float readFixed() const noexcept { return getFloat(wpos_); }
};
