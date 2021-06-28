/// @file      device/pca9555.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
namespace pca9555
{
constexpr uint8_t LEVEL_METER = 0x20 << 1;     ///< レベルメータースレーブアドレス
constexpr uint8_t ROTARY_ENCODER = 0x21 << 1;  ///< ロータリーエンコーダスレーブアドレス
constexpr uint8_t INPUT_0 = 0x00;              ///<
constexpr uint8_t INPUT_1 = 0x01;              ///<
constexpr uint8_t OUTPUT_0 = 0x02;             ///<
constexpr uint8_t OUTPUT_1 = 0x03;             ///<
constexpr uint8_t POLARITY_INVERSION_0 = 0x04; ///<
constexpr uint8_t POLARITY_INVERSION_1 = 0x05; ///<
constexpr uint8_t CONFIGURATION_0 = 0x06;      ///<
constexpr uint8_t CONFIGURATION_1 = 0x07;      ///<
} // namespace pca9555
} // namespace satoh
