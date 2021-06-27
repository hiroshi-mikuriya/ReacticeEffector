/// @file      device/pca9635.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "common/rgb.h"
#include "i2c.h"

namespace satoh
{
class PCA9635;
}

class satoh::PCA9635
{
  /// I2C通信オブジェクト
  I2C *const i2c_;
  /// @brief I2C書き込み
  /// @param[in] reg 書き込み先のレジスタ
  /// @param[in] bytes 書き込みデータ
  /// @param[in] size 書き込みサイズ
  bool write(uint8_t reg, uint8_t const *bytes, uint32_t size);

public:
  /// @brief コンストラクタ
  /// @param[in] i2c I2C通信オブジェクト
  explicit PCA9635(I2C *i2c) noexcept;
  /// @brief デストラクタ
  virtual ~PCA9635();
  /// @brief LEDの色値を設定
  /// @param[in] rgb 色値
  /// @param[in] n LED番号（0 - 3）
  /// @retval true 設定成功
  /// @retval false I2C通信失敗、LED番号不正等
  bool set(RGB const &rgb, uint8_t n);
};