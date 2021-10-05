/// @file      state/error.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "error.h"
#include "common/utils.h"
#include "user.h"

namespace msg = satoh::msg;
namespace state = satoh::state;

state::ID state::Error::run(msg::MODE_KEY const *src) noexcept
{
  if (src->ok == msg::BUTTON_DOWN)
  {
  }
  if (src->rtn == msg::BUTTON_DOWN)
  {
  }
  if (src->up == msg::BUTTON_DOWN)
  {
  }
  if (src->down == msg::BUTTON_DOWN)
  {
  }
  if (src->tap == msg::BUTTON_DOWN)
  {
  }
  if (src->re1 == msg::BUTTON_DOWN)
  {
  }
  return id();
}
state::ID state::Error::run(msg::EFFECT_KEY const *src) noexcept
{
  for (uint8_t i = 0; i < countof(src->button); ++i)
  {
  }
  return id();
}
state::ID state::Error::run(msg::ACC_GYRO const *src) noexcept
{
  return id();
}
state::ID state::Error::run(msg::ROTARY_ENCODER const *src) noexcept
{
  return id();
}
state::ID state::Error::run(msg::ERROR const *src) noexcept
{
  return id();
}
state::ID state::Error::timer() noexcept
{
  return id();
}
void state::Error::init() noexcept
{
  msg::OLED_DISP_TEXT txt{};
  sprintf(txt.msg1, "ERROR : %d", m_.getError());
  switch (m_.getError())
  {
  default:
    strcpy(txt.msg2, "UNDEFINED");
    break;
  case msg::error::ROTARY_ENCODER:
    strcpy(txt.msg2, "ROTARY ENCODER");
    break;
  case msg::error::GYRO:
    strcpy(txt.msg2, "GYRO");
    break;
  case msg::error::EFFECT_LED:
    strcpy(txt.msg2, "EFFECT LED");
    break;
  case msg::error::LEVEL_METER:
    strcpy(txt.msg2, "LEVEL METER");
    break;
  case msg::error::MODE_KEY:
    strcpy(txt.msg2, "MODE KEY");
    break;
  case msg::error::OLED: // OLEDが故障していたらエラー表示できない・・・
    strcpy(txt.msg2, "OLED");
    break;
  case msg::error::I2C_1:
  case msg::error::I2C_2:
  case msg::error::I2C_3:
    strcpy(txt.msg2, "I2C");
    break;
  case msg::error::SPI_1:
  case msg::error::SPI_2:
  case msg::error::SPI_3:
  case msg::error::SPI_4:
  case msg::error::SPI_5:
  case msg::error::SPI_6:
    strcpy(txt.msg2, "SPI");
    break;
  case msg::error::ADC_1:
  case msg::error::ADC_2:
  case msg::error::ADC_3:
    strcpy(txt.msg2, "ADC");
    break;
  case msg::error::UART_1:
  case msg::error::UART_2:
  case msg::error::UART_3:
  case msg::error::UART_4:
  case msg::error::UART_5:
  case msg::error::UART_6:
  case msg::error::UART_7:
  case msg::error::UART_8:
    strcpy(txt.msg2, "UART");
    break;
  }
  msg::send(i2cTaskHandle, msg::OLED_DISP_TEXT_REQ, txt);
}
void state::Error::deinit() noexcept {}
