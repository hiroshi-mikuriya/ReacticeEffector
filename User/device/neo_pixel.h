/// @file      device/neo_pixel.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#include "common/rgb.h"
#include "peripheral/spi_master.h"

namespace satoh
{
class NeoPixel;
} // namespace satoh

/// @brief Neo Pixel 制御クラス
class satoh::NeoPixel
{
  SpiMaster *spi_;          ///< SPI通信クラス
  const uint32_t ledCount_; ///< LED数
  Alloc<uint8_t> buf_;      ///< 送信バッファ
public:
  /// @brief コンストラクタ
  /// @param[in] spi SPI通信クラス
  /// @param[in] ledCount LED数
  NeoPixel(SpiMaster *spi, uint32_t ledCount) noexcept;
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
