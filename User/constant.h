/// @file      constant.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstddef> // size_t
#include <cstdint> // uint8_t

namespace satoh
{
constexpr size_t MAX_EFFECTOR_COUNT = 3;           ///< 同時に実行できる最大エフェクター数
constexpr uint8_t MAX_BANK = 4;                    ///< バンク最大値
constexpr uint8_t MAX_PATCH = 4;                   ///< パッチ最大値
constexpr uint8_t MAX_PARAM_COUNT = 6;             ///< １つのエフェクターが持つパラメータ数最大値
constexpr size_t EFFECT_LED_COUNT = 4;             ///< エフェクトLED数
constexpr size_t EFFECT_BUTTON_COUNT = 4;          ///< エフェクトボタン数
constexpr float SAMPLING_FREQ = 44.433f * 1000.0f; ///< サンプリング周期
constexpr float PI = 3.141592653589793f;           ///< 円周率
constexpr uint32_t BLOCK_SIZE = 96;                ///< 音声信号ブロックサイズ

} // namespace satoh