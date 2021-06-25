/// @file      task/neo_pixel_task.h
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  /// @brief neoPixelTask内部処理
  /// @param[in] argument タスク引数
  void nepPixelTaskImpl(void const *argument);

  /// @brief NeoPixel DMA送信完了割り込み
  void neoPixelTxEndIRQ(void);

  /// @brief NeoPixel DMA送信エラー割り込み
  void neoPixelTxErrorIRQ(void);

#ifdef __cplusplus
}
#endif
