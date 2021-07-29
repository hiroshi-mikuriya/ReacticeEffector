/// @file      state/property.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/alloc.hpp"
#include "constant.h"
#include "effector/effector_base.h"
#include "message/error.h"
#include <memory>

namespace satoh
{
namespace state
{
class Effectors;
struct EffectParam;
struct Patch;
struct PatchTable;
class Property;
} // namespace state
} // namespace satoh

/// @brief エフェクター一覧型
class satoh::state::Effectors
{
  /// @brief デフォルトコンストラクタ削除
  Effectors() = delete;
  /// エフェクターポインタ型
  using Ptr = UniquePtr<fx::EffectorBase>;
  /// エフェクター一覧
  Ptr list_[16];
  /// エフェクター数
  size_t count_;
  /// @brief エフェクターを一覧に追加する
  /// @tparam FX エフェクター種類
  /// @tparam Args エフェクタークラスコンストラクタ引数型
  /// @param[in] cond 追加条件（trueならば追加する）
  /// @param[in] args エフェクタークラスコンストラクタ引数
  template <typename FX, class... Args>
  void addList(bool cond, Args... args)
  {
    if (cond)
    {
      Ptr ptr(alloc<FX>(args...));
      if (*ptr)
      {
        list_[count_++] = std::move(ptr);
      }
    }
  }

public:
  /// @brief コンストラクタ
  /// @param[in] n FX番号（0, 1, 2）
  explicit Effectors(uint8_t n) noexcept;
  /// @brief エフェクター取得
  /// @param[in] i インデックス
  /// @return エフェクター
  fx::EffectorBase *getFx(size_t i) noexcept;
  /// @brief エフェクター取得
  /// @param[in] i インデックス
  /// @return エフェクター
  fx::EffectorBase const *getFx(size_t i) const noexcept;
  /// @brief エフェクター取得
  /// @param[in] id エフェクターID
  /// @return エフェクター
  fx::EffectorBase *getFxById(fx::ID id) noexcept;
  /// @brief エフェクター取得
  /// @param[in] id エフェクターID
  /// @return エフェクター
  fx::EffectorBase const *getFxById(fx::ID id) const noexcept;
  /// @brief エフェクター数を取得
  /// @return エフェクター数
  size_t count() const noexcept;
  /// @brief エフェクター一覧から検索してインデックスを返す
  /// @param[in] fx 検索対象のエフェクター
  /// @retval 0以上 インデックス
  /// @retval -1 見つからない
  /// @retval -2 引数がNULL
  int find(fx::EffectorBase const *fx);
  /// @brief 次のエフェクターを取得
  /// @param[in] cur 現在選択中のエフェクター
  /// @param[in] next
  ///   @arg true 次のエフェクターを検索
  ///   @arg false 前のエフェクターを検索
  /// @return 次 or 前のエフェクター
  fx::EffectorBase *getNext(fx::EffectorBase const *cur, bool next) noexcept;
};

/// @brief １つのエフェクターが持つデータ
struct satoh::state::EffectParam
{
  fx::ID id;      ///< エフェクターID
  bool gyro[6];   ///< ジャイロ有効・無効
  float value[6]; ///< パラメータ値
  /// @brief FXからパラメータを読み込む
  /// @param[in] fx 読み込み先
  void read(fx::EffectorBase const *fx) noexcept;
  /// @brief エフェクターにパラメータを書き込む
  /// @param[in] fx 書き込み先
  void write(fx::EffectorBase *fx) const noexcept;
};

/// @brief １つのパッチが持つデータ
struct satoh::state::Patch
{
  /// パラメータ
  EffectParam param[MAX_EFFECTOR_COUNT];
};

/// @brief 全パッチ
struct satoh::state::PatchTable
{
  /// CRC（データ破損判定）
  uint32_t crc_;
  /// パッチデータ
  Patch m_[MAX_BANK][MAX_PATCH];
  /// @brief CRCを計算
  /// @return CRC計算結果
  uint32_t calcCrc() const noexcept;
};

/// @brief 状態プロパティ
class satoh::state::Property
{
  /// バンク番号
  uint8_t bankNum_;
  /// パッチ番号
  uint8_t patchNum_;
  /// エフェクター一覧
  Effectors effectors_[MAX_EFFECTOR_COUNT];
  /// パッチデータ
  PatchTable *patches_;
  /// 現在有効になっているエフェクター
  fx::EffectorBase *fx_[MAX_EFFECTOR_COUNT];
  /// 編集中のエフェクト番号
  uint8_t editSelectedFxNum_;
  /// エラー原因
  msg::error::ID error_;
  /// @brief パッチをロードする
  void loadPatch() noexcept;

public:
  /// 最後にタップを押した時間
  uint32_t lastTapTick = 0;
  /// @brief コンストラクタ
  /// @param[in] パッチテーブル
  explicit Property(PatchTable *patch);
  /// @brief エフェクター番号を指定して取得
  /// @param[in] n 0, 1, 2
  /// @return エフェクター
  fx::EffectorBase *getFx(uint8_t n) noexcept;
  /// @brief エフェクター番号を指定して取得
  /// @param[in] n 0, 1, 2
  /// @return エフェクター
  fx::EffectorBase const *getFx(uint8_t n) const noexcept;
  /// @brief 選択中のエフェクターを取得
  /// @return エフェクター
  fx::EffectorBase *getEditSelectedFx() noexcept;
  /// @brief 選択中のエフェクターを取得
  /// @return エフェクター
  fx::EffectorBase const *getEditSelectedFx() const noexcept;
  /// @brief バンク番号更新
  /// @param[in] up
  ///   @arg true +1
  ///   @arg false -1
  /// @note 変更前のパッチを保存し、変更後のパッチを読み出す
  void changeBank(bool up) noexcept;
  /// @brief パッチ番号更新
  /// @param[in] n パッチ番号
  /// @note 変更前のパッチを保存し、変更後のパッチを読み出す
  void changePatch(uint8_t n) noexcept;
  /// @brief バンク番号取得
  /// @return バンク番号
  uint8_t getBankNum() const noexcept;
  /// @brief パッチ番号取得
  /// @return パッチ番号
  uint8_t getPatchNum() const noexcept;
  /// @brief 選択中のパッチを取得
  /// @return 選択中のパッチ
  Patch &getCurrectPatch() noexcept;
  /// @brief 選択中のパッチを取得
  /// @return 選択中のパッチ
  Patch const &getCurrectPatch() const noexcept;
  /// @brief 現在のパッチの色を取得
  /// @return パッチ色
  RGB getCurrentColor() const noexcept;
  /// @brief パッチを保存する
  void savePatch() noexcept;
  /// @brief 選択中のFX1-3のエフェクタを変更する
  /// @param[in] up
  ///   @arg true +1
  ///   @arg false -1
  void updateNextFx(bool up) noexcept;
  /// @brief 編集中のエフェクト番号更新
  /// @param[in] up
  ///   @arg true +1
  ///   @arg false -1
  void updateEditSelectedFxNum(bool up) noexcept;
  /// @brief 編集中のエフェクト番号を取得
  /// @return 編集中のエフェクト番号
  uint8_t getEditSelectedFxNum() const noexcept;
  /// @brief 編集中のエフェクト番号を0に戻す
  void initEditSelectedFxNum() noexcept;
  /// @brief ファクトリーリセットする
  void factoryReset() noexcept;
  /// @brief エラー原因を取得する @return エラー原因
  msg::error::ID getError() const noexcept { return error_; }
  /// @brief エラー原因を設定する @param[in] e エラー原因
  void setError(msg::error::ID e) noexcept { error_ = e; }
};
