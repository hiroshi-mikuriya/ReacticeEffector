/// @file      effector/id.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
namespace fx
{
/// @brief エフェクターID
using ID = uint16_t;
/// @brief エフェクターカテゴリ
namespace cat
{
constexpr uint16_t SHIFT = 12;
constexpr ID DIST = 1 << SHIFT;   ///< 歪み系エフェクター
constexpr ID FILTER = 2 << SHIFT; ///< 周波数特性系エフェクター
constexpr ID SPACE = 3 << SHIFT;  ///< 空間系エフェクター
constexpr ID PITCH = 4 << SHIFT;  ///< ピッチ系エフェクター
constexpr ID NOISE = 5 << SHIFT;  ///< ノイズ除去系エフェクター
constexpr ID OTHER = 15 << SHIFT; ///< その他のエフェクター
} // namespace cat
/// ディストーション
constexpr ID DISTORTION = 1 | cat::DIST;
/// オーバードライブ
constexpr ID OVERDRIVE = 2 | cat::DIST;
/// ブースター
constexpr ID BOOSTER = 3 | cat::DIST;
/// コーラス
constexpr ID CHORUS = 1 | cat::SPACE;
/// フェイザー
constexpr ID PHASER = 2 | cat::SPACE;
/// トレモロ
constexpr ID TREMOLO = 3 | cat::SPACE;
/// RAMディレイ
constexpr ID DELAY_RAM = 4 | cat::SPACE;
/// SPIディレイ
constexpr ID DELAY_SPI = 5 | cat::SPACE;
/// リバーブ
constexpr ID REVERB = 6 | cat::SPACE;
/// BQフィルター
constexpr ID BQ_FILTER = 1 | cat::FILTER;
/// バイパス
constexpr ID BYPASS = 1 | cat::OTHER;
/// テンプレート
constexpr ID TEMPLATE = 2 | cat::OTHER;
/// オシレーター
constexpr ID OSCILLATOR = 3 | cat::OTHER;
} // namespace fx
} // namespace satoh