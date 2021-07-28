/// @file      message/error.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include <cstdint>

namespace satoh
{
namespace msg
{
namespace error
{
/// @brief エラーID型
typedef uint16_t ID;
/// @brief メッセージカテゴリ定義
namespace cat
{
constexpr ID SHIFT = 12;
constexpr ID DEVICE = 1 << SHIFT;
constexpr ID PERIPHERAL = 2 << SHIFT;
} // namespace cat

constexpr ID NONE = 0;                         ///< エラーなし
constexpr ID GYRO = 1 | cat::DEVICE;           ///< ジャイロエラー
constexpr ID ROTARY_ENCODER = 2 | cat::DEVICE; ///< ロータリーエンコーダエラー
constexpr ID LEVEL_METER = 3 | cat::DEVICE;    ///< レベルメーターエラー
constexpr ID MODE_KEY = 4 | cat::DEVICE;       ///< モードキーエラー
constexpr ID EFFECT_LED = 5 | cat::DEVICE;     ///< エフェクトLEDエラー
constexpr ID OLED = 6 | cat::DEVICE;           ///< OLEDエラー
constexpr ID I2C_1 = 1 | cat::PERIPHERAL;      ///< I2C_1エラー
constexpr ID I2C_2 = 2 | cat::PERIPHERAL;      ///< I2C_2エラー
constexpr ID I2C_3 = 3 | cat::PERIPHERAL;      ///< I2C_3エラー
constexpr ID SPI_1 = 11 | cat::PERIPHERAL;     ///< SPI_1エラー
constexpr ID SPI_2 = 12 | cat::PERIPHERAL;     ///< SPI_2エラー
constexpr ID SPI_3 = 13 | cat::PERIPHERAL;     ///< SPI_3エラー
constexpr ID SPI_4 = 14 | cat::PERIPHERAL;     ///< SPI_4エラー
constexpr ID SPI_5 = 15 | cat::PERIPHERAL;     ///< SPI_5エラー
constexpr ID SPI_6 = 16 | cat::PERIPHERAL;     ///< SPI_6エラー
constexpr ID ADC_1 = 21 | cat::PERIPHERAL;     ///< ADC_1エラー
constexpr ID ADC_2 = 22 | cat::PERIPHERAL;     ///< ADC_2エラー
constexpr ID ADC_3 = 23 | cat::PERIPHERAL;     ///< ADC_3エラー
constexpr ID UART_1 = 31 | cat::PERIPHERAL;    ///< UART_1エラー
constexpr ID UART_2 = 32 | cat::PERIPHERAL;    ///< UART_2エラー
constexpr ID UART_3 = 33 | cat::PERIPHERAL;    ///< UART_3エラー
constexpr ID UART_4 = 34 | cat::PERIPHERAL;    ///< UART_4エラー
constexpr ID UART_5 = 35 | cat::PERIPHERAL;    ///< UART_5エラー
constexpr ID UART_6 = 36 | cat::PERIPHERAL;    ///< UART_6エラー
constexpr ID UART_7 = 37 | cat::PERIPHERAL;    ///< UART_7エラー
constexpr ID UART_8 = 38 | cat::PERIPHERAL;    ///< UART_8エラー
} // namespace error
} // namespace msg
} // namespace satoh
