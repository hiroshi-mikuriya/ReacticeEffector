/// @file      task/usb_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "cmsis_os.h"
#include "handles.h"
#include "message/type.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

namespace msg = satoh::msg;

namespace
{
constexpr int32_t SIG_USBTXEND = 1 << 0;
}

extern "C"
{
  /// @brief USB送信Task内部処理
  /// @param [in] argument 未使用
  void usbTxTaskProc(void const *argument)
  {
    UNUSED(argument);
    MX_USB_DEVICE_Init();
    if (msg::registerThread(2) != osOK)
    {
      return;
    }
    for (;;)
    {
      auto res = msg::recv();
      if (res.status() != osOK)
      {
        continue;
      }
      auto *msg = res.msg();
      if (msg == 0)
      {
        continue;
      }
      if (msg->type != msg::USB_TX_REQ)
      {
        continue;
      }
      if (CDC_Transmit_FS(const_cast<uint8_t *>(msg->bytes), msg->size) != USBD_OK)
      {
        continue;
      }
      res.reset();
      osSignalWait(SIG_USBTXEND, 10);
    }
  }
  /// @brief USB受信割り込み
  /// @param [in] bytes 受信データの先頭ポインタ
  /// @param [in] size 受信データサイズ
  void usbRxIRQ(uint8_t const *bytes, uint32_t size) { msg::send(usbTxTaskHandle, msg::USB_RX_NOTIFY, bytes, size); }
  /// @brief USB送信完了割り込み
  /// @param [in] bytes 受信データの先頭ポインタ
  /// @param [in] size 受信データサイズ
  /// @param [in] epnum エンドポイント
  void usbTxEndIRQ(uint8_t const *bytes, uint32_t size, uint8_t epnum)
  {
    UNUSED(bytes);
    UNUSED(size);
    UNUSED(epnum);
    osSignalSet(usbTxTaskHandle, SIG_USBTXEND);
  }
} // extern "C"