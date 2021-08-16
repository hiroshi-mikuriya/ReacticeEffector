/// @file      effector/lib/lib_delay.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/alloc.hpp"
#include "constant.h"
#include "lib_float.hpp"
#include <algorithm>

namespace satoh
{
template <typename T>
class delayBuf;
} // namespace satoh

/// @brief ディレイバッファ
/// @tparam T 保持するデータ型（int8_t, int16_t or float）
template <typename T>
class satoh::delayBuf
{
  /// @brief コピーコンストラクタ削除
  delayBuf(delayBuf const &) = delete;
  /// @brief 代入演算子削除
  delayBuf &operator=(delayBuf const &) = delete;

  /// @brief 書込位置と読出位置の間隔を計算 @param[in] ms 時間
  static constexpr float getInterval(float ms) { return 0.001f * ms * satoh::SAMPLING_FREQ; }
  /// @brief バッファサイズ取得
  /// @param[in] ms 最大保持時間（ミリ秒）
  /// @return バッファサイズ
  static constexpr uint32_t getBufferSize(float ms) { return 1 + static_cast<uint32_t>(getInterval(ms)); }

  uint32_t maxSize_; ///< バッファ最大要素数
  float interval_;   ///< インターバル
  UniquePtr<T> buf_; ///< バッファ
  uint32_t wpos_;    ///< 書き込み位置

  /// @brief float値を変換して保存する
  /// @param[in] pos 格納先のインデックス
  /// @param[in] v 値
  void setFloat(uint32_t pos, float v) noexcept { buf_.get()[pos] = fromFloat<T>(v); }
  /// @brief 値を読み出して、floatへ変換して取得する
  /// @param[in] pos 読み出し先のインデックス
  /// @return float値
  float getFloat(uint32_t pos) const noexcept { return toFloat<T>(buf_.get()[pos]); }
  /// @brief 読み込み位置を取得する @return 読み込み位置
  float getRpos() const noexcept { return interval_ <= wpos_ ? wpos_ - interval_ : wpos_ + maxSize_ - interval_; }

public:
  /// @brief コンストラクタ
  delayBuf() noexcept : maxSize_(0), interval_(1), wpos_(0) {}
  /// @brief コンストラクタ
  /// @param[in] maxTime 最大保持時間（ミリ秒）
  explicit delayBuf(float maxTime) noexcept //
      : maxSize_(getBufferSize(maxTime)),   //
        interval_(1),                       //
        buf_(allocArray<T>(maxSize_)),      //
        wpos_(0)                            //
  {
    memset(buf_.get(), 0, maxSize_ * sizeof(T));
  }
  /// @brief moveコンストラクタ
  explicit delayBuf(delayBuf &&) = default;
  /// @brief move演算子
  delayBuf &operator=(delayBuf &&that) = default;
  /// @brief デストラクタ
  virtual ~delayBuf() {}
  /// @brief メモリ確保成功・失敗を取得
  /// @retval true 成功
  /// @retval false 失敗
  explicit operator bool() const noexcept { return static_cast<bool>(buf_); }
  /// @brief インターバルを設定する @param[in] ms 時間（ミリ秒）
  void setInterval(float ms) noexcept { interval_ = std::min(static_cast<float>(maxSize_), getInterval(std::max(1.0f, ms))); }
  /// @brief バッファ配列書き込み、書込位置を進める
  /// @param[in] v 書き込むデータ
  void write(float v) noexcept
  {
    setFloat(wpos_, v);
    wpos_ = (wpos_ + 1) % maxSize_;
  }
  /// @brief 通常のサンプル単位での読み出し
  float read() const noexcept { return getFloat(static_cast<uint32_t>(getRpos())); }
  /// @brief 線形補間して読み出し（コーラス等に利用）
  float readLerp() const noexcept
  {
    float rpos = getRpos();
    uint32_t rpos0 = static_cast<uint32_t>(rpos);
    uint32_t rpos1 = (rpos0 + 1) % maxSize_;
    float t = rpos - rpos0;
    return (1 - t) * getFloat(rpos0) + t * getFloat(rpos1);
  }
  /// @brief 固定時間（最大ディレイタイム）で読み出し
  float readFixed() const noexcept { return getFloat(wpos_); }
};
