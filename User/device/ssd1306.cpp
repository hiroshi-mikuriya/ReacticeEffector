/// @file      device/ssd1306.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "ssd1306.h"
#include "common/utils.h"
#include "fonts.h"
#include <cstdio>  // sprintf
#include <cstring> // memset

namespace
{
constexpr char const *BYPASS_NAME = "---";

constexpr uint8_t SLAVE_ADDR = 0x3C;

constexpr uint8_t WIDTH = 128;                  ///< 横ピクセル数
constexpr uint8_t NUM_PAGE = 8;                 ///< ページ数
constexpr uint8_t HEIGHT = NUM_PAGE * 8;        ///< 縦ピクセル数
constexpr uint32_t BUF_SIZE = WIDTH * NUM_PAGE; ///< 画面の全ピクセルデータをバッファするのに必要なサイズ

constexpr uint8_t SSD1306_CONFIG_MUX_RATIO_CMD = 0xA8;
constexpr uint8_t SSD1306_CONFIG_MUX_RATIO_A = 0x3F;
constexpr uint8_t SSD1306_CONFIG_DISPLAY_OFFSET_CMD = 0xD3;
constexpr uint8_t SSD1306_CONFIG_DISPLAY_OFFSET_A = 0x0;
constexpr uint8_t SSD1306_CONFIG_DISPLAY_START_LINE = 0x40;
constexpr uint8_t SSD1306_CONFIG_SEGMENT_REMAP = 0xA1;
constexpr uint8_t SSD1306_CONFIG_COM_OUT_DIRECTION = 0xC8;
constexpr uint8_t SSD1306_CONFIG_COM_PIN_CONFIG_CMD = 0xDA;
constexpr uint8_t SSD1306_CONFIG_COM_PIN_CONFIG_A = 0x12;
constexpr uint8_t SSD1306_CONFIG_CONTRAST_CMD = 0x81;
constexpr uint8_t SSD1306_CONFIG_CONTRAST_A = 0x7F;
constexpr uint8_t SSD1306_CONFIG_ENTIRE_DISPLAY_ON = 0xA4;
constexpr uint8_t SSD1306_CONFIG_DISPLAY_PIX_MODE = 0xA6;
constexpr uint8_t SSD1306_CONFIG_DISPLAY_FREQ_CMD = 0xD5;
constexpr uint8_t SSD1306_CONFIG_DISPLAY_FREQ_A = 0xF0;
constexpr uint8_t SSD1306_CONFIG_ADDRESSING_MODE_CMD = 0x20;
constexpr uint8_t SSD1306_CONFIG_ADDRESSING_MODE_A = 0x0;
constexpr uint8_t SSD1306_CONFIG_CHARGE_PUMP_CMD = 0x8D;
constexpr uint8_t SSD1306_CONFIG_CHARGE_PUMP_A = 0x14;
constexpr uint8_t SSD1306_CONFIG_DISPLAY_ON_OFF = 0xAF;

///< 1つのコマンドセットのみ
constexpr uint8_t SSD1306_CTRL_BYTE_CMD_SINGLE = 0b00000000;
///< コマンドの後ろに続けて複数のコントロールバイト＆コマンドor描画データを場合
constexpr uint8_t SSD1306_CTRL_BYTE_CMD_STREAM = 0b10000000;
///< 描画データのWriteのみ
constexpr uint8_t SSD1306_CTRL_BYTE_DATA_SINGLE = 0b01000000;
///< 描画データの後ろに続けて複数のコントロールバイト＆コマンドor描画データを場合
constexpr uint8_t SSD1306_CTRL_BYTE_DATA_STREAM = 0b11000000;

/// @brief １ピクセル書き込む
/// @param [in] color 色
/// @param [in] x X位置
/// @param [in] y Y位置
/// @param [out] dst 書き込み先
inline void drawPixel(uint8_t color, uint8_t x, uint8_t y, uint8_t *dst) noexcept
{
  if (x < WIDTH && y < HEIGHT)
  {
    if (color)
    {
      dst[x + (y / 8) * WIDTH] |= 1 << (y % 8);
    }
    else
    {
      dst[x + (y / 8) * WIDTH] &= ~(1 << (y % 8));
    }
  }
}
/// @brief １文字書き込む
/// @param [in] ch １文字
/// @param [in] font フォントデータ
/// @param [in] invert 反転有無
/// @param [in] x X位置
/// @param [in] y Y位置
/// @param [out] dst 書き込み先
inline void drawChar(char ch, satoh::FontDef const &font, bool invert, uint8_t x, uint8_t y, uint8_t *dst) noexcept
{
  for (uint8_t dy = 0; dy < font.height; ++dy)
  {
    uint16_t b = font.data[(ch - 32) * font.height + dy];
    for (uint8_t dx = 0; dx < font.width; ++dx)
    {
      uint8_t color = (b << dx) & 0x8000 ? 1 : 0;
      if (invert)
      {
        color = !color;
      }
      drawPixel(color, x + dx, y + dy, dst);
    }
  }
}
/// @brief 文字列を書き込む
/// @param [in] str 文字列
/// @param [in] font フォントデータ
/// @param [in] invert 反転有無
/// @param [in] x X位置
/// @param [in] y Y位置
/// @param [out] dst 書き込み先
inline void drawString(char const *str, satoh::FontDef const &font, bool invert, uint8_t x, uint8_t y, uint8_t *dst) noexcept
{
  for (char const *p = str; *p; ++p)
  {
    drawChar(*p, font, invert, x, y, dst);
    x += font.width;
  }
}
} // namespace

bool satoh::SSD1306::init() const noexcept
{
  // see. https://monoedge.net/raspi-ssd1306/
  // 1	Set MUX Ratio	使用する行数。
  // default値0x3F=64行	0xA8, 0x3F
  // 2	Set Display Offset	画面垂直方向のオフセット(ずらし)
  // オフセットは設けないので0に設定	0xD3, 0x00
  // 3	Set Display Start Line	画面スタートライン
  // 全領域使用するので0x40を指定(0x40~0x7F)	0x40
  // 4	Set Segment Re-map	水平方向の反転
  // デフォルトは右下スタートだが、今回は左上スタートにするので反転有効=0xA1	0xA1
  // 5	Set COM Output Scan Direction	垂直方向の反転
  // デフォルトは右下スタートだが、今回は左上スタートにするので反転有効=0xC8	0xC8
  // 6	Set COM Pins Hardware Configuration	COM signals pinの設定のようだが値を変えても挙動に変化無し。デフォルト値0x12でOK	0xDA, 0x12
  // 7	Set Contrast Control	256階調のコントラスト(明るさ)設定
  // デフォルトは0x7F。最大は0xFF。お好みでOK	0x81, 0xFF
  // 8	Disable Entire Display On	0xA4で画面メモリの内容を画面表示
  // 0xA5でテスト用(と思われる)の全画面表示機能なので使用しない	0xA4
  // 9	Set Normal Display	白黒反転設定
  // 通常は1が発光だが、その逆の設定も可能
  // 今回は通常設定	0xA6
  // 10	Set Osc Frequency	ディスプレイのクロック設定
  // デフォルト値0x80でOK	0xD5, 0x80
  // 11	Set Addressing Mode	アドレスモードの設定
  // 0x00：水平方向アドレッシング←今回はこれ
  // 0x01：垂直方向アドレッシング
  // 0x10：ページアドレッシング(default)	0x20, 0x00
  // 12	Enable Charge Pump Regulator	ディスプレイをONにするにはチャージポンプレギュレータをONにしておく必要があるので設定します	0x8D, 0x14
  // 13	Display On	0xAEは画面OFF
  // 0xAFで画面ONです	0xAF

  constexpr uint8_t v[] = {
      SSD1306_CTRL_BYTE_CMD_SINGLE,       //
      SSD1306_CONFIG_MUX_RATIO_CMD,       //
      SSD1306_CONFIG_MUX_RATIO_A,         //
      SSD1306_CONFIG_DISPLAY_OFFSET_CMD,  //
      SSD1306_CONFIG_DISPLAY_OFFSET_A,    //
      SSD1306_CONFIG_DISPLAY_START_LINE,  //
      SSD1306_CONFIG_SEGMENT_REMAP,       //
      SSD1306_CONFIG_COM_OUT_DIRECTION,   //
      SSD1306_CONFIG_COM_PIN_CONFIG_CMD,  //
      SSD1306_CONFIG_COM_PIN_CONFIG_A,    //
      SSD1306_CONFIG_CONTRAST_CMD,        //
      SSD1306_CONFIG_CONTRAST_A,          //
      SSD1306_CONFIG_ENTIRE_DISPLAY_ON,   //
      SSD1306_CONFIG_DISPLAY_PIX_MODE,    //
      SSD1306_CONFIG_DISPLAY_FREQ_CMD,    //
      SSD1306_CONFIG_DISPLAY_FREQ_A,      //
      SSD1306_CONFIG_ADDRESSING_MODE_CMD, //
      SSD1306_CONFIG_ADDRESSING_MODE_A,   //
      SSD1306_CONFIG_CHARGE_PUMP_CMD,     //
      SSD1306_CONFIG_CHARGE_PUMP_A,       //
      SSD1306_CONFIG_DISPLAY_ON_OFF,      //
  };
  // 失敗すると画面が消えてしまうが次回成功で復帰するので、適当に通信をリトライしておく。
  for (int i = 0; i < 10; ++i)
  {
    if (write(v, sizeof(v)))
    {
      return true;
    }
    osDelay(1);
  }
  return false;
}

bool satoh::SSD1306::sendBufferToDevice() noexcept
{
  init();

  for (uint8_t page = 0; page < NUM_PAGE; ++page)
  {
    uint8_t v0[] = {
        SSD1306_CTRL_BYTE_CMD_STREAM, //
        (uint8_t)(0xB0 + page),       // set page start address
        SSD1306_CTRL_BYTE_CMD_SINGLE, //
        0x21,                         // set column address
        0,                            // column start address(0-127)
        WIDTH - 1,                    // column stop address(0-127)
    };
    write(v0, sizeof(v0));

    uint8_t *tx = txbuf_.get();
    tx[0] = SSD1306_CTRL_BYTE_DATA_SINGLE;
    memcpy(&tx[1], getDispBuffer() + page * WIDTH, WIDTH);
    write(tx, WIDTH + 1);
  }
  return true;
}

satoh::SSD1306::SSD1306(I2C *i2c) noexcept        //
    : I2CDeviceBase(i2c, SLAVE_ADDR),             //
      buffer_(allocArray<uint8_t>(BUF_SIZE + 1)), //
      txbuf_(allocArray<uint8_t>(WIDTH + 32)),    //
      ok_(init())                                 //
{
  if (ok_)
  {
    sendBufferToDevice();
  }
}

bool satoh::SSD1306::update(msg::OLED_DISP_EFFECTOR const &src) noexcept
{
  uint8_t *disp = getDispBuffer();
  memset(disp, 0, BUF_SIZE);
  satoh::FontDef const &titleFont = satoh::Font_11x18;
  satoh::FontDef const &paramFont = satoh::Font_7x10;
  if (!src.fx)
  {
    drawString(BYPASS_NAME, titleFont, false, 0, 0, disp);
    return sendBufferToDevice();
  }
  {
    // タイトル表示
    char txt[16] = {0};
    sprintf(txt, "FX%d ", src.patch);
    drawString(txt, paramFont, false, 0, 0, disp);
    drawString(src.fx->getName(), titleFont, false, paramFont.width * 4, 0, disp);
  }
  uint8_t n = 0;
  for (uint8_t col = 0; col < 2; ++col)
  {
    for (uint8_t row = 0; row < 3 && n < src.fx->getParamCount(); ++row, ++n)
    {
      uint8_t y = titleFont.height + 6 + row * (paramFont.height + 5);
      uint8_t cx = col * WIDTH / 2;
      const char *key = src.fx->getParamName(n);
      drawString(key, paramFont, n == src.selectedParam, cx, y, disp);
      const char *value = src.fx->getValueTxt(n);
      size_t len = strlen(value);
      uint8_t px = cx + 43;
      if (2 < len)
      {
        px -= paramFont.width * (len - 2); // 3ケタ以上の数値を左にずらして全体が表示されるようにする
      }
      drawString(value, paramFont, false, px, y, disp);
    }
  }
  return sendBufferToDevice();
}
bool satoh::SSD1306::update(msg::OLED_DISP_BANK const &src) noexcept
{
  uint8_t *disp = getDispBuffer();
  memset(disp, 0, BUF_SIZE);
  satoh::FontDef const &titleFont = satoh::Font_11x18;
  satoh::FontDef const &paramFont = satoh::Font_7x10;
  char txt[16] = {0};
  sprintf(txt, "%d - %d", src.bank, src.patch);
  drawString(txt, titleFont, false, 10, 0, disp);
  if (src.editMode)
  {
    drawString("EDIT", titleFont, true, titleFont.width * 7, 0, disp);
  }
  for (size_t i = 0; i < countof(src.fx); ++i)
  {
    constexpr uint8_t margin = 5;
    uint8_t y = titleFont.height + margin + (paramFont.height + margin) * i;
    uint8_t patch = i + 1;
    int n = sprintf(txt, "FX%d:", i + 1);
    drawString(txt, paramFont, src.editMode && src.selectedFx == patch, 0, y, disp);
    const char *name = src.fx[i] ? src.fx[i]->getName() : BYPASS_NAME;
    drawString(name, paramFont, false, paramFont.width * (n + 1), y, disp);
  }
  return sendBufferToDevice();
}
bool satoh::SSD1306::update(msg::OLED_DISP_CONFIRM const &src) noexcept
{
  const auto &font0 = Font_7x10;
  const auto &font1 = Font_11x18;
  uint8_t *disp = getDispBuffer();
  memset(disp, 0, BUF_SIZE);
  drawString(src.msg1, font0, false, 0, font0.height * 0, disp);
  drawString(src.msg2, font0, false, 0, font0.height * 1, disp);
  drawString("YES", font1, src.yes, 10, font0.height * 3, disp);
  drawString("NO", font1, !src.yes, 80, font0.height * 3, disp);
  return sendBufferToDevice();
}
bool satoh::SSD1306::update(msg::OLED_DISP_TEXT const &src) noexcept
{
  const auto &font = Font_7x10;
  uint8_t *disp = getDispBuffer();
  memset(disp, 0, BUF_SIZE);
  drawString(src.msg1, font, false, 0, font.height * 1, disp);
  drawString(src.msg2, font, false, 0, font.height * 2, disp);
  drawString(src.msg3, font, false, 0, font.height * 3, disp);
  return sendBufferToDevice();
}
bool satoh::SSD1306::update(msg::OLED_DISP_TUNER const &src) noexcept
{
  const auto &font = Font_7x10;
  uint8_t *disp = getDispBuffer();
  memset(disp, 0, BUF_SIZE);
  drawString("TUNER", font, false, 40, 0, disp);
  if (src.estimated)
  {
    drawString(src.name, Font_16x26, false, 50, font.height * 1 + 6, disp);
    drawString(" ", font, true, font.width * (8 + src.diff), font.height * 5, disp);
  }
  drawString(" <----- = -----> ", font, false, 0, font.height * 4, disp);
  return sendBufferToDevice();
}
