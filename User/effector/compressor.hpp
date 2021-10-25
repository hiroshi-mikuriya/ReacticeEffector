/// @file      effector/compressor.hpp
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
class Compressor;
}
} // namespace satoh

/// @brief Sodiumから拝借したコンプレッサー
class satoh::fx::Compressor : public satoh::fx::EffectorBase
{
  enum
  {
    LEVEL,     ///< レベル
    THRESHOLD, ///< 閾値
    RATIO,     ///< 比率
    ATTACK,    ///< アタック
    RELEASE,   ///< リリース
    KNEE,      ///< ニー
    COUNT,     ///< パラメータ総数
  };

  EffectParameterF ui_[COUNT]; ///< UIから設定するパラメータ
  mutable char valueTxt_[8];   ///< パラメータ文字列格納バッファ
  lpf lpfEnv;                  ///< エンベロープ用ローパスフィルタ
  lpf lpfAtkRel;               ///< アタック・リリース用ローパスフィルタ
  float level_;                ///< LEVEL 0～+30dB
  float threshold_;            ///< THRESHOLD -90～0dB
  float ratio_;                ///< RATIO 1:Infinity, 1:2～1:10
  float attack_;               ///< ATTACK 4～100ms(LPF周波数1～25Hz)
  float release_;              ///< RELEASE 5～400ms(LPF周波数1～80Hz)
  float knee_;                 ///< KNEE 0～20dB

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param [in] n 変換対象のパラメータ番号
  void convUiToFx(uint8_t n) noexcept override
  {
    float v = ui_[n].getValue();
    switch (n)
    {
    case LEVEL:
      level_ = logPot(v, 0.0f, 30.0f);
      break;
    case THRESHOLD:
      threshold_ = v;
      break;
    case RATIO:
      if (v != 0)
      {
        ratio_ = v <= 10 ? 1.0f / v : 0.0f;
      }
      break;
    case ATTACK:
      if (v != 0)
      {
        attack_ = 100.0f / v;
      }
      break;
    case RELEASE:
      if (v != 0)
      {
        release_ = 400.0f / v;
      }
      break;
    case KNEE:
      knee_ = v;
      break;
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
    case THRESHOLD:
    case ATTACK:
    case RELEASE:
    case KNEE:
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    case RATIO:
      if (10 < ui_[RATIO].getValue())
      {
        return "Inf";
      }
      sprintf(valueTxt_, "%d", static_cast<int>(ui_[n].getValue()));
      return valueTxt_;
    default:
      return 0;
    }
  }

public:
  /// @brief コンストラクタ
  Compressor()
      : EffectorBase(COMPRESSOR, "Compressor", "CS", RGB{0x00, 0x00, 0x20}), // エフェクター名、短縮エフェクター名、LED色
        ui_({
            EffectParameterF(0, 100, 1, "LEVEL"), //
            EffectParameterF(-90, 0, 1, "THRE"),  //
            EffectParameterF(2, 11, 1, "RATIO"),  //
            EffectParameterF(4, 100, 1, "ATK"),   //
            EffectParameterF(5, 400, 1, "REL"),   //
            EffectParameterF(0, 20, 1, "KNEE"),   //
        })                                        //
  {
    lpfEnv.set(500.0f); // エンベロープ用ローパスフィルタ設定
    lpfAtkRel.set(100.0f);
    init(ui_, COUNT);
  }
  /// @brief デストラクタ
  virtual ~Compressor() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param [in] size 音声データ数
  void effect(float *left, float *right, uint32_t size) noexcept override
  {
    for (uint32_t i = 0; i < size; ++i)
    {
      float fx = right[i];
      float th = threshold_;     // スレッショルド 一時変数
      float dbGain = 0.0f;       // コンプレッション(音量圧縮)幅 dB
      float env = abs(fx);       // エンベロープ
      env = lpfEnv.process(env); // 絶対値とLPFでエンベロープ取得
      env = gainToDb(env);       // dB換算
      if (env < th - knee_)
      {
        dbGain = 0.0f;
        lpfAtkRel.set(release_);            // リリース用LPF周波数を設定
        dbGain = lpfAtkRel.process(dbGain); // リリース用LPFで音量変化を遅らせる
      }
      else
      {
        if (env < th + knee_) // THRESHOLD ± KNEE の範囲では2次関数の曲線を使用
        {
          th = -0.25f / knee_ * (env - th - knee_) * (env - th - knee_) + th;
        }
        dbGain = (env - th) * ratio_ + th - env; // 音量圧縮幅計算
        lpfAtkRel.set(attack_);                  // アタック用LPF周波数を設定
        dbGain = lpfAtkRel.process(dbGain);      // アタック用LPFで音量変化を遅らせる
      }
      fx = dbToGain(dbGain) * fx; // コンプレッション実行
      fx = level_ * fx;           // LEVEL
      right[i] = fx;
    }
  }
};
