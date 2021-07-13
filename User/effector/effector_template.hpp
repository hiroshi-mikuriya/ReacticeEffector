/// @file      effector/effector_template.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include <cstdio> // sprintf

namespace satoh
{
class EffectorTemplate;
}

/// @brief エフェクタークラスの実装参考用テンプレート
class satoh::EffectorTemplate : public satoh::EffectorBase
{
  enum
  {
    PARAM0 = 0, ///< パラメータ0
    PARAM1,     ///< パラメータ1
    PARAM2,     ///< パラメータ2
    COUNT,      ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  float param0_;               ///< エフェクト処理で使用するパラメータ0
  float param1_;               ///< エフェクト処理で使用するパラメータ1
  float param2_;               ///< エフェクト処理で使用するパラメータ2

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case PARAM0:
      param0_ = ui_[PARAM0].getValue(); // TODO 加工して代入する
      break;
    case PARAM1:
      param1_ = ui_[PARAM1].getValue(); // TODO 加工して代入する
      break;
    case PARAM2:
      param2_ = ui_[PARAM2].getValue(); // TODO 加工して代入する
      break;
    }
  }
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override
  {
    switch (n)
    {
    case PARAM0:
    case PARAM1:
    case PARAM2:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  EffectorTemplate()
      : EffectorBase("FX Template", "TM", RGB{0x20, 0x20, 0x20}), // エフェクター名、短縮エフェクター名、LED色
        ui_({
            EffectParameterF(1, 100, 40, 1, "P0"), // 最小値1, 最大値100, 初期値40, ステップ1, 表示文字
            EffectParameterF(1, 100, 1, "P1"),     // 最小値1, 最大値100, 初期値省略（最大最小の中間値）, ステップ1, 表示文字
            EffectParameterF(1, 100, 60, 1, "P2"), // 最小値1, 最大値100, 初期値60, ステップ1, 表示文字
        }),                                        //
        param0_(0),                                //
        param1_(0),                                //
        param2_(0)                                 //
  {
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~EffectorTemplate() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      // TODO param0_, param1_, param2_を使って、エフェクト処理する
    }
  }
};
