/// @file      message/msglib.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "cmsis_os.h"
#include "type.h"

namespace satoh
{
struct Message;
class Result;

/// @brief 自タスクをメッセージ送信先に追加
/// @param[in] msgCount 格納できる最大メッセージ数
/// @retval osOK 成功
/// @retval それ以外 失敗理由
osStatus addMsgTarget(uint32_t msgCount) noexcept;
/// @brief メッセージ送信
/// @param[in] threadId 送信先タスクID
/// @param[in] type メッセージ種別
/// @retval osOK 送信成功
/// @retval osErrorParameter 引数エラー
/// @retval osEventTimeout タイムアウト
/// @retval osErrorOS 送信失敗
osStatus sendMsg(osThreadId threadId, uint8_t type) noexcept;
/// @brief メッセージ送信
/// @param[in] threadId 送信先タスクID
/// @param[in] type メッセージ種別
/// @param[in] bytes データ先頭ポインタ
/// @param[in] size データサイズ
/// @retval osOK 送信成功
/// @retval osErrorParameter 引数エラー
/// @retval osEventTimeout タイムアウト
/// @retval osErrorOS 送信失敗
osStatus sendMsg(osThreadId threadId, uint8_t type, void const *bytes, uint16_t size) noexcept;
/// @brief メッセージ受信
/// @param[in] millisec タイムアウト時間
/// @return 受信結果
Result recvMsg(uint32_t millisec = osWaitForever) noexcept;
} // namespace satoh

/// @brief メッセージ型
struct satoh::Message
{
  uint8_t type;       ///< メッセージ種別
  uint16_t size;      ///< データサイズ
  uint8_t bytes[512]; ///< データ
};

/// @brief 受信結果型
class satoh::Result
{
  /// コピーコンストラクタ削除
  Result(Result const &) = delete;
  /// コピー代入演算子削除
  Result &operator=(Result const &) = delete;

  osStatus status_; ///< 受信ステータス
  Message *msg_;    ///< メッセージ
  osMailQId mail_;  ///< メールID

public:
  /// @brief コンストラクタ
  /// @param[in] status 受信ステータス
  /// @param[in] msg メッセージ
  explicit Result(osStatus status, Message *msg, osMailQId mail) noexcept;
  /// @brief コンストラクタ
  /// @param[in] status 受信ステータス
  explicit Result(osStatus status) noexcept;
  /// @brief デストラクタ
  virtual ~Result();
  /// @brief moveコンストラクタ
  Result(Result &&that) noexcept;
  /// @brief move代入演算子
  /// @param[in] that 移動元
  /// @return インスタンスのポインタ
  Result &operator=(Result &&that) noexcept;
  /// @brief msg_のメモリを開放する
  void reset() noexcept;
  /// @brief ステータス取得
  /// @return ステータス
  osStatus status() const noexcept;
  /// @brief メッセージ取得
  /// @return メッセージ
  Message const *msg() const noexcept;
};