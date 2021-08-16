/// @file      effector/lib/lib_delay_spi.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/dma_mem.h"
#include "common/little_endian.hpp"
#include "constant.h"
#include "lib_float.hpp"
#include "peripheral/spi_master.h"
#include <algorithm>

namespace satoh
{
template <typename T>
class delaySpiBuf;
} // namespace satoh

/// @brief ディレイバッファ
/// @tparam T 保持するデータ型（int8_t, int16_t or float）
template <typename T>
class satoh::delaySpiBuf
{
  /// @brief コピーコンストラクタ削除
  delaySpiBuf(delaySpiBuf const &) = delete;
  /// @brief 代入演算子削除
  delaySpiBuf &operator=(delaySpiBuf const &) = delete;

  enum
  {
    CMD_WRITE = 2,  ///< SPI SRAM WRITEコマンド
    CMD_READ = 3,   ///< SPI SRAM READコマンド
    HEADER_SIZE = 4 ///< SPI SRAM 通信ヘッダーサイズ
  };

  /// @brief コマンドサイズを取得する @param[in] count データ数 @return コマンドサイズ
  static constexpr uint32_t getCommandSize(uint32_t count) noexcept { return HEADER_SIZE + count * sizeof(T); }
  /// @brief 書込位置と読出位置の間隔を計算 @param[in] ms 時間
  static constexpr uint32_t getInterval(float ms) { return static_cast<uint32_t>(1e-3f * ms * satoh::SAMPLING_FREQ); }

  SpiMaster *spi_;              ///< SPI通信オブジェクト
  const uint32_t blockSize_;    ///< ブロックサイズ
  const uint32_t endPos_;       ///< 終了位置
  uint32_t interval_;           ///< インターバル
  UniqueDmaPtr<uint8_t> txbuf_; ///< 送信バッファ
  UniqueDmaPtr<uint8_t> rxbuf_; ///< 受信バッファ
  uint32_t wpos_;               ///< 書き込み位置

  /// @brief データをfloatからT型に変換し、SRAMへの書き込みをおこなう
  /// @param[in] floats 書き込みデータ（T型へ変換する前のfloatデータ）
  /// @param[in] size floatデータ数
  /// @param[in] pos 書き込み位置
  /// @retval true 書き込み成功
  /// @retval false 書き込み失敗
  bool writeSram(float const *floats, size_t size, uint32_t pos) const noexcept
  {
    if (size == 0)
    {
      return true;
    }
    uint8_t *t = txbuf_.get();
    LE<uint32_t>::set(t, pos * sizeof(T));
    t[0] = CMD_WRITE;
    T *p = reinterpret_cast<T *>(t + HEADER_SIZE);
    for (size_t i = 0; i < size; ++i)
    {
      p[i] = fromFloat<T>(floats[i]);
    }
    return spi_->send(t, getCommandSize(size)) == SpiMaster::OK;
  }
  /// @brief SPI SRAMからT型のデータを読み出し、floatに変換する
  /// @param[in] buffer 読み込み先のバッファ
  /// @param[in] size 読み込みデータ数
  /// @param[in] pos 読み込み位置
  /// @retval true 読み込み成功
  /// @retval false 読み込み失敗
  bool readSram(float *buffer, size_t size, uint32_t pos) const noexcept
  {
    if (size == 0)
    {
      return true;
    }
    uint32_t cmdSize = getCommandSize(size);
    uint8_t *t = txbuf_.get();
    uint8_t *r = rxbuf_.get();
    memset(t, 0, cmdSize);
    memset(r, 0, cmdSize);
    LE<uint32_t>::set(t, pos * sizeof(T));
    t[0] = CMD_READ;
    if (spi_->sendRecv(t, r, cmdSize) != SpiMaster::OK)
    {
      return false;
    }
    T const *p = reinterpret_cast<T const *>(r + HEADER_SIZE);
    for (size_t i = 0; i < size; ++i)
    {
      buffer[i] = toFloat<T>(p[i]);
    }
    return true;
  }

public:
  /// @brief デフォルトコンストラクタ
  delaySpiBuf() noexcept : spi_(0), blockSize_(0), endPos_(0), interval_(1), wpos_(0) {}
  /// @brief コンストラクタ
  /// @param[in] spi SPI通信オブジェクト
  /// @param[in] blockSize ブロックサイズ
  /// @param[in] maxTime 最大保持時間（ミリ秒）
  explicit delaySpiBuf(SpiMaster *spi, uint32_t blockSize, float maxTime) noexcept //
      : spi_(spi),                                                                 //
        blockSize_(blockSize),                                                     //
        endPos_(getInterval(maxTime)),                                             //
        interval_(1),                                                              //
        txbuf_(makeDmaMem<uint8_t>(getCommandSize(blockSize_))),                   //
        rxbuf_(makeDmaMem<uint8_t>(getCommandSize(blockSize_))),                   //
        wpos_(0)                                                                   //
  {
    memset(txbuf_.get(), 0, getCommandSize(blockSize_));
    memset(rxbuf_.get(), 0, getCommandSize(blockSize_));
  }
  /// @brief デストラクタ
  virtual ~delaySpiBuf() {}
  /// @brief メモリ確保成功・失敗を取得
  /// @retval true 成功
  /// @retval false 失敗
  explicit operator bool() const noexcept { return txbuf_ && rxbuf_; }
  /// @brief インターバルを設定する @param[in] ms 時間（ミリ秒）
  void setInterval(float ms) noexcept { interval_ = std::min(endPos_, getInterval(std::max(1.0f, ms))); }
  /// @brief 今回処理した音声信号をSRAMに書き込む
  /// @param[in] floats 音声データ（ブロックサイズ分）
  void write(float const *floats) noexcept
  {
    const uint32_t dpos = endPos_ - wpos_;
    if (dpos < blockSize_)
    {
      writeSram(floats, dpos, wpos_);
      writeSram(floats + dpos, blockSize_ - dpos, 0);
    }
    else
    {
      writeSram(floats, blockSize_, wpos_);
    }
    wpos_ = (wpos_ + blockSize_) % endPos_;
  }
  /// @brief 今回使用する音声をSRAMから読み出す
  /// @param[in] buffer 格納先のバッファ（ブロックサイズ分）
  void read(float *buffer) const noexcept
  {
    const uint32_t rpos = (wpos_ + endPos_ - interval_) % endPos_;
    const uint32_t dpos = endPos_ - rpos;
    if (dpos < blockSize_)
    {
      readSram(buffer, dpos, rpos);
      readSram(buffer + dpos, blockSize_ - dpos, 0);
    }
    else
    {
      readSram(buffer, blockSize_, rpos);
    }
  }
};
