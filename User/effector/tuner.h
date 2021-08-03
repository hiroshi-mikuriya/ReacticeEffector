/// @file      effector/tuner.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/alloc.hpp"
#include "effector_base.h"
#include "lib/lib_filter.hpp"

namespace satoh
{
namespace fx
{
class Tuner;
}
} // namespace satoh

/// @brief チューナー
class satoh::fx::Tuner : public satoh::fx::EffectorBase
{
  lpf2nd lpf_;                       ///< 入力2次LPF
  UniquePtr<float> inData_[2];       ///< 入力音の配列 2つ準備し交互に利用
  uint16_t inDataCnt_;               ///< 入力音配列の添字カウント
  UniquePtr<uint32_t> bitStream_[2]; ///< ビットストリーム配列 2つ準備し交互に利用
  uint8_t arraySel_;                 ///< 2つの配列 入れ替え用
  UniquePtr<uint32_t> corrArray_;    ///< correlation(相間) 配列
  uint32_t maxCorr_;                 ///< correlation(相間) 最大値
  uint32_t minCorr_;                 ///< correlation(相間) 最小値
  uint16_t estimatedIndex_;          ///< 推定周期 サンプル数
  float estimatedFreq_;              ///< 推定周波数
  uint32_t lastUpdateTime_;          ///< 最後に推定周波数を更新した時刻

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override {}
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override { return 0; }

public:
  /// @brief コンストラクタ
  Tuner();
  /// @brief デストラクタ
  virtual ~Tuner() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override;
  /// @brief 推定周波数を取得する @return 推定周波数
  float getEstimatedFreq() const noexcept { return estimatedFreq_; }
  /// @brief 最後に推定周波数を更新した時刻を取得する @return 最後に推定周波数を更新した時刻
  uint32_t getLastUpdateTime() const noexcept { return lastUpdateTime_; }
};
