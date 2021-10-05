/// @file      task/usb_task.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "usb_task.h"
#include "cmsis_os.h"
#include "message/type.h"
#include "usbd_cdc_if.h"

namespace msg = satoh::msg;

namespace
{
constexpr int32_t SIG_USBTXEND = 1 << 0;
}

void usbTxTaskProc(void const *argument)
{
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

void usbRxIRQ(uint8_t const *bytes, uint32_t size)
{
  // TODO
}

void usbTxEndIRQ(uint8_t const *bytes, uint32_t size)
{
  osSignalSet(usbTxTaskHandle, SIG_USBTXEND);
}
