/// @file      effector/bypass.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class Bypass;
}
} // namespace satoh

/// @brief バイパス
class satoh::fx::Bypass : public satoh::fx::EffectorBase
{
  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param [in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    // do nothing
  }
  /// @brief パラメータ値文字列取得
  /// @param [in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override { return 0; }

public:
  /// @brief コンストラクタ
  Bypass() : EffectorBase(BYPASS, "---", "--", RGB{0x00, 0x00, 0x00}) //
  {
    init(0, 0);
  }
  /// @brief デストラクタ
  virtual ~Bypass() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param [in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    // do nothing
  }
};
