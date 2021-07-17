/// @file      effector/effector_base.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include "id.h"
#include <cstdint>
#include <cstring> // strlen, memcpy

namespace satoh
{
class EffectorBase;
template <typename T>
class EffectParameter;
/// エフェクトパラメータ型（float版）
typedef EffectParameter<float> EffectParameterF;

/// @brief データ圧縮
/// @param[in] min 最小値
/// @param[inout] v 圧縮対象データ
/// @param[in] max 最大値
/// @retval true 圧縮した
/// @retval false 圧縮しなかった
inline bool compress(float min, float &v, float max) noexcept
{
  if (v < min)
  {
    v = min;
    return true;
  }
  if (max < v)
  {
    v = max;
    return true;
  }
  return false;
}
} // namespace satoh

/// @brief エフェクトパラメータが共通で持つ属性
/// @tparam T パラメータの型
template <typename T>
class satoh::EffectParameter
{
  const T min_;        ///< 最小値
  const T max_;        ///< 最大値
  T v_;                ///< 値
  const T step_;       ///< 目盛り
  const char *name_;   ///< パラメータ名
  bool isGyroEnabled_; ///< ジャイロ制御有無 @arg true 制御あり @arg false 制御なし

  /// @brief データ圧縮
  /// @retval true 圧縮した
  /// @retval false 圧縮しなかった
  bool compress() noexcept { return satoh::compress(min_, v_, max_); }

public:
  /// @brief コンストラクタ
  /// @param[in] min 最小値
  /// @param[in] max 最大値
  /// @param[in] v 初期値
  /// @param[in] step 目盛り
  /// @param[in] name パラメータ名
  explicit EffectParameter(T min, T max, T v, T step, const char *name) noexcept //
      : min_(min), max_(max), v_(v), step_(step), name_(name), isGyroEnabled_(false)
  {
  }
  /// @brief コンストラクタ（初期値は最大と最小の中間値にする）
  /// @param[in] min 最小値
  /// @param[in] max 最大値
  /// @param[in] step 目盛り
  /// @param[in] name パラメータ名
  explicit EffectParameter(T min, T max, T step, const char *name) noexcept //
      : EffectParameter(min, max, (max + min) / 2, step, name)
  {
  }
  /// @brief 値取得
  /// @return 値
  T getValue() const noexcept { return v_; }
  /// @brief 最大値取得
  /// @return 最大値
  T getMax() const noexcept { return max_; }
  /// @brief 最小値取得
  /// @return 最小値
  T getMin() const noexcept { return min_; }
  /// @brief 目盛り取得
  /// @return 目盛り
  T getStep() const noexcept { return step_; }
  /// @brief パラメータ名取得
  /// @return 文字列長
  const char *getName() const noexcept { return name_; }
  /// @brief 加算する
  /// @retval true 加算された
  /// @retval false 最大値に到達しているため加算されなかった
  bool increment() noexcept { return setValue(v_ + step_); }
  /// @brief 減算する
  /// @retval true 減算された
  /// @retval false 最小値に到達しているため減算されなかった
  bool decrement() noexcept { return setValue(v_ - step_); }
  /// @brief 値を設定する
  /// @param[in] v 値
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  bool setValue(T v) noexcept
  {
    satoh::compress(min_, v, max_);
    if (v_ == v)
    {
      return false;
    }
    v_ = v;
    return true;
  }
  /// @brief 値を比率で設定する
  /// @param[in] ratio 比率（最小値 0.0f 〜 1.0f 最大値）
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  bool setValueRatio(float ratio) noexcept { return setValue((max_ - min_) * ratio + min_); }
  /// @brief ジャイロ制御有無を設定する
  /// @param[in] enable ジャイロ制御有無
  void setGyroEnable(bool enable) noexcept { isGyroEnabled_ = enable; }
  /// @brief ジャイロ制御有無を取得する
  /// @retval true ジャイロ制御あり
  /// @retval false ジャイロ制御なし
  bool isGyroEnabled() const noexcept { return isGyroEnabled_; }
};

/// @brief エフェクター基底クラス
class satoh::EffectorBase
{
  /// ON/OFF状態
  bool isActive_;
  /// UIパラメータ
  EffectParameterF *uiParam_;
  /// パラメータ総数
  uint8_t paramCount_;
  /// エフェクターID
  const fx::ID id_;
  /// エフェクター名
  const char *name_;
  /// エフェクター名（短縮）
  const char *shortName_;
  /// LED色
  const RGB ledColor_;

  /// @brief UI表示のパラメータを、エフェクト処理で使用する値へ変換する
  /// @param[in] n 変換対象のパラメータ番号
  virtual void convUiToFx(uint8_t n) noexcept = 0;
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  virtual const char *getValueTxtImpl(uint8_t n) const noexcept = 0;

protected:
  /// @brief 属性初期化
  /// @param[in] uiParam UIパラメータ
  /// @param[in] paramCount パラメータ総数
  /// @note 継承先のコンストラクタから呼び出すこと
  void init(EffectParameterF *uiParam, uint8_t paramCount)
  {
    uiParam_ = uiParam;
    paramCount_ = paramCount;
    for (uint8_t n = 0; n < paramCount_; ++n)
    {
      convUiToFx(n);
    }
  }

public:
  /// @brief コンストラクタ
  /// @param[in] id エフェクターID
  /// @param[in] name エフェクター名
  /// @param[in] shortName エフェクター名（短縮）
  /// @param[in] ledColor アクティブ時のLED色
  explicit EffectorBase(fx::ID id, const char *name, const char *shortName, RGB const &ledColor) //
      : isActive_(false), uiParam_(0), paramCount_(0), id_(id), name_(name), shortName_(shortName), ledColor_(ledColor)
  {
  }
  /// @brief デストラクタ
  virtual ~EffectorBase() {}
  /// @brief エフェクト処理実行
  /// @param[inout] left L音声データ
  /// @param[inout] right R音声データ
  /// @param[in] size 音声データ数
  virtual void effect(float *left, float *right, uint32_t size) noexcept = 0;
  /// @brief ON/OFF状態を取得
  /// @retval true ON
  /// @retval false OFF
  virtual bool isActive() const noexcept { return isActive_; }
  /// @brief ON/OFF設定
  /// @param[in] active
  ///   @arg true ON
  ///   @arg false OFF
  virtual void setActive(bool active) noexcept { isActive_ = active; }
  /// @brief ON/OFF設定切替
  virtual void toggleActive() noexcept { isActive_ = !isActive_; }
  /// @brief エフェクターIDを取得
  /// @return エフェクターID
  virtual fx::ID getID() const noexcept { return id_; }
  /// @brief エフェクト名を取得
  /// @return 文字列のポインタ
  virtual const char *getName() const noexcept { return name_; }
  /// @brief 短縮したエフェクト名を取得
  /// @return 文字列のポインタ
  virtual const char *getShortName() const noexcept { return shortName_; }
  /// @brief パラメータ数を取得
  /// @return パラメータ数
  virtual uint8_t getParamCount() const noexcept { return paramCount_; }
  /// @brief パラメータ取得
  /// @param[in] n 取得対象のパラメータ番号
  /// @return パラメータ
  virtual float getParam(uint8_t n) const noexcept
  {
    if (n < paramCount_)
    {
      return uiParam_[n].getValue();
    }
    return 0;
  }
  /// @brief パラメータ設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] v 値
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  virtual bool setParam(uint8_t n, float v) noexcept
  {
    if (n < paramCount_)
    {
      if (uiParam_[n].setValue(v))
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief ジャイロ連携有無取得
  /// @param[in] n パラメータ番号
  /// @retval true ジャイロ連携あり
  /// @retval false ジャイロ連携なし
  virtual bool isGyroEnabled(uint8_t n) const noexcept
  {
    if (n < paramCount_)
    {
      return uiParam_[n].isGyroEnabled();
    }
    return false;
  }
  /// @brief ジャイロ連携設定
  /// @param[in] n パラメータ番号
  /// @param[in] enable
  ///    @arg true ジャイロ連携あり
  ///    @arg false ジャイロ連携なし
  virtual void setGyroEnable(uint8_t n, bool enable) noexcept
  {
    if (n < paramCount_)
    {
      uiParam_[n].setGyroEnable(enable);
    }
  }
  /// @brief ジャイロセンサーの加速度値からパラメータを設定する
  /// @param[in] acc 加速度値
  /// @note setGyroEnable(true)にした値のみが設定される
  virtual void setGyroParam(int16_t const (&acc)[3]) noexcept
  {
    float ratio = (acc[1] + 0x8000) / 65536.0f; // TODO 暫定
    for (uint8_t n = 0; n < paramCount_; ++n)
    {
      if (uiParam_[n].isGyroEnabled())
      {
        setParamRatio(n, ratio);
      }
    }
  }
  /// @brief パラメータ加算
  /// @param[in] n 加算対象のパラメータ番号
  /// @retval true 加算した
  /// @retval false 最大値に到達しているため加算しなかった
  virtual bool incrementParam(uint8_t n) noexcept
  {
    if (n < paramCount_)
    {
      if (uiParam_[n].increment())
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ減算
  /// @param[in] n 減算対象のパラメータ番号
  /// @retval true 減算した
  /// @retval false 最小値に到達しているため減算しなかった
  virtual bool decrementParam(uint8_t n) noexcept
  {
    if (n < paramCount_)
    {
      if (uiParam_[n].decrement())
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ比率設定
  /// @param[in] n 設定対象のパラメータ番号
  /// @param[in] ratio 比率（最小値 0.0f 〜 1.0f 最大値）
  /// @retval true 設定された
  /// @retval false 元々の値と同じだったため設定されなかった
  virtual bool setParamRatio(uint8_t n, float ratio) noexcept
  {
    if (n < paramCount_)
    {
      if (uiParam_[n].setValueRatio(ratio))
      {
        convUiToFx(n);
        return true;
      }
    }
    return false;
  }
  /// @brief パラメータ名文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  virtual const char *getParamName(uint8_t n) const noexcept
  {
    if (n < paramCount_)
    {
      return uiParam_[n].getName();
    }
    return 0;
  }
  /// @brief パラメータ値文字列取得
  /// @param[in] n パラメータ番号
  /// @return 文字列のポインタ
  virtual const char *getValueTxt(uint8_t n) const noexcept
  {
    if (n < paramCount_)
    {
      if (uiParam_[n].isGyroEnabled())
      {
        return "GY";
      }
      return getValueTxtImpl(n);
    }
    return 0;
  }
  /// @brief LED色を取得
  /// @return LED色
  virtual RGB getColor() const noexcept { return isActive_ ? ledColor_ : RGB{}; }
};
