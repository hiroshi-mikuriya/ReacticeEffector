/// @file      device/i2c.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"
#include "stm32f7xx.h"

namespace satoh
{
class I2C;
} // namespace satoh

/// @brief I2C通信クラス
class satoh::I2C
{
public:
  /// @brief 関数リターン値定義
  enum Result
  {
    OK = 0,  ///< 成功
    BUSY,    ///< ビジー
    TIMEOUT, ///< タイムアウト
    NAK,     ///< NAK
  };
  /// @brief コンストラクタ
  /// @param[in] i2cx I2Cペリフェラル
  /// @param[in] threadId イベント通知先のスレッドID
  explicit I2C(I2C_TypeDef *const i2cx, osThreadId threadId) noexcept;
  /// @brief デストラクタ
  virtual ~I2C();
  /// @brief 割り込み許可
  void enableIRQ();
  /// @brief 割り込み禁止
  void disableIRQ();
  /// @brief イベント割り込みが発生したら呼び出す関数
  void notifyEvIRQ();
  /// @brief エラー割り込みが発生したら呼び出す関数
  void notifyErIRQ();
  /// @brief レジスタにデータを書き込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] reg レジスタ番号
  /// @param[in] v 書き込むデータ
  Result writeByte(uint8_t slaveAddr, uint8_t reg, uint8_t v) noexcept;
  /// @brief レジスタからデータを１バイト読み込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] reg レジスタ番号
  /// @param[out] v 読み込んだ値の格納先
  /// @return 読み込んだデータ
  Result readByte(uint8_t slaveAddr, uint8_t reg, uint8_t &v) noexcept;
  /// @brief レジスタからデータを複数バイト読み込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] reg レジスタ番号
  /// @param[out] buf 読み込んだデータを格納するバッファ
  /// @param[in] size 読み込むバイト数
  Result readBytes(uint8_t slaveAddr, uint8_t reg, uint8_t *buf, uint8_t size) noexcept;

private:
  /// I2Cペリフェラル
  I2C_TypeDef *const i2cx_;
  /// イベント通知先のスレッドID
  osThreadId threadId_;
  /// 読み込みバッファ
  uint8_t *rxbuf_;
  /// 書き込みバッファ
  uint8_t const *txbuf_;
  /// @brief データを複数バイト書き込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] bytes 書き込みデータの先頭ポインタ
  /// @param[in] size 書き込むバイト数
  Result writeImpl(uint8_t slaveAddr, uint8_t const *bytes, uint8_t size) noexcept;
  /// @brief データを複数バイト読み込む
  /// @param[in] slaveAddr スレーブアドレス
  /// @param[in] buf 読み込んだデータを格納するバッファ
  /// @param[in] size 読み込むバイト数
  Result readImpl(uint8_t slaveAddr, uint8_t *buffer, uint8_t size) noexcept;
  /// @brief レジスタをクリアする
  void clearRegister() const noexcept;
};