/// @file      device/neo_pixel.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include "stm32f7xx.h"
#include <memory>

namespace satoh
{
class NeoPixel;
} // namespace satoh

/// @brief Neo Pixel 制御クラス
class satoh::NeoPixel
{
  SPI_TypeDef *const spi_;       ///< SPIペリフェラル
  DMA_TypeDef *const dma_;       ///< 送信DMA
  const uint32_t stream_;        ///< 送信DMAストリーム
  const uint32_t ledCount_;      ///< LED数
  std::unique_ptr<uint8_t> buf_; ///< DMA送信バッファ

public:
  /// @brief コンストラクタ
  /// @param[in] spi SPIペリフェラル
  /// @param[in] dma 送信DMA
  /// @param[in] stream 送信DMAストリーム
  /// @param[in] ledCount LED数
  NeoPixel(SPI_TypeDef *spi, DMA_TypeDef *dma, uint32_t stream, uint32_t ledCount) noexcept;
  /// @brief デストラクタ
  virtual ~NeoPixel();
  /// @brief 色値を設定する（この関数を呼んだだけでは光らない）
  /// @param[in] rgb 色値
  /// @param[in] n LED番号
  void set(RGB const &rgb, uint32_t n) noexcept;
  /// @brief 色値を全てクリアする（この関数を呼んだだけでは光らない）
  void clear() noexcept;
  /// @brief NeoPixelを光らせる
  /// @retval true NeoPixelへのデータ転送成功
  /// @retval false 前回の転送が完了していないため転送できなかった
  bool show() const noexcept;
};
