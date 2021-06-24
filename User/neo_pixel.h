/// @file      neo_pixel.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include <cstdint>

namespace sato
{
class NeoPixel;
}

/// @brief Neo Pixel 制御クラス
class sato::NeoPixel
{
  static constexpr uint32_t N = 24;
  uint8_t buf_[N * 3 * 8];

public:
  /// @brief コンストラクタ
  NeoPixel() noexcept;
  /// @brief デストラクタ
  virtual ~NeoPixel();
};
