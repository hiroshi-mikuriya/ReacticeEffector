/// @file      effector/overdrive.hpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "effector_base.h"
#include "lib/lib_calc.hpp"
#include "lib/lib_filter.hpp"
#include <cstdio> // sprintf

namespace satoh
{
namespace fx
{
class OverDrive;
}
} // namespace satoh

/// @brief Sodiumから拝借したオーバードライブ
class satoh::fx::OverDrive : public satoh::fx::EffectorBase
{
  enum
  {
    LEVEL = 0, ///< レベル
    GAIN,      ///< ゲイン
    TREBLE,    ///< トレブル
    BASS,      ///< ベース
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  hpf hpfFixed;                ///< 出力ローカット
  hpf hpfBass;                 ///< 入力BASS調整
  lpf lpfFixed;                ///< 入力ハイカット
  lpf lpfTreble;               ///< 出力TREBLE調整
  float level_;                ///< レベル
  float gain_;                 ///< ゲイン

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param [in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    switch (n)
    {
    case LEVEL:
      level_ = logPot(ui_[LEVEL].getValue(), -50.0f, 0.0f); // LEVEL -50～0 dB
      break;
    case GAIN:
      gain_ = logPot(ui_[GAIN].getValue(), 20.0f, 60.0f); // GAIN 20～60 dB
      break;
    case TREBLE:
    {
      float treble = 10000.0f * logPot(ui_[TREBLE].getValue(), -30.0f, 0.0f); // TREBLE LPF 320～10k Hz
      lpfTreble.set(treble);
      break;
    }
    case BASS:
    {
      float bass = 2000.0f * logPot(ui_[BASS].getValue(), 0.0f, -20.0f); // BASS HPF 200～2000 Hz
      hpfBass.set(bass);
      break;
    }
    }
  }
  /// @brief パラメータ値文字列取得
  /// @param [in] n パラメータ番号
  /// @return 文字列のポインタ
  const char *getValueTxtImpl(uint8_t n) const noexcept override
  {
    switch (n)
    {
    case LEVEL:
    case GAIN:
    case TREBLE:
    case BASS:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  OverDrive()                                                              //
      : EffectorBase(OVERDRIVE, "Overdrive", "OD", RGB{0x20, 0x20, 0x00}), //
        ui_{
            EffectParameterF(1, 100, 1, "LEVEL"),  //
            EffectParameterF(1, 100, 1, "GAIN"),   //
            EffectParameterF(1, 100, 1, "TREBLE"), //
            EffectParameterF(1, 100, 1, "BASS"),   //
        },                                         //
        level_(0),                                 //
        gain_(0)                                   //
  {
    lpfFixed.set(4000.0f); // 入力ハイカット 固定値
    hpfFixed.set(30.0f);   // 出力ローカット 固定値
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~OverDrive() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param [in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      fx = hpfBass.process(fx);   // 入力ローカット BASS
      fx = lpfFixed.process(fx);  // 入力ハイカット 固定値
      fx *= gain_;                // GAIN
      fx = atanf(fx + 0.5f);      // arctanによるクリッピング、非対称化
      fx = hpfFixed.process(fx);  // 出力ローカット 固定値 直流カット
      fx = lpfTreble.process(fx); // 出力ハイカット TREBLE
      fx *= level_;               // LEVEL
      right[i] = fx;
    }
  }
};
