#include <Adafruit_SSD1306.h>
#include <efont.h>
#include <efontEnableJaMini.h>

namespace displayManager {
const char *menuTxt[99];
const char *wearerTxt[99];
const char *decibelTxt[99];

void setTitle(const char **menu, int m_size, const char **wearer, int w_size,
              const char **decibel, int d_size) {
  for (int i = 0; i < m_size; i++) {
    menuTxt[i] = menu[i];
  }
  for (int i = 0; i < w_size; i++) {
    wearerTxt[i] = wearer[i];
  }
  for (int i = 0; i < d_size; i++) {
    decibelTxt[i] = decibel[i];
  }
}

// rot for rotation (deg) of display. usually 0 or 90.
void initOLED(Adafruit_SSD1306 *display, int rot) {
  if (!(*display).begin(SSD1306_SWITCHCAPVCC)) {
    USBSerial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }
  (*display).clearDisplay();
  (*display).setRotation(rot);
  (*display).display();
}

// 日本語描画
void printEfont(Adafruit_SSD1306 *display, const char *str, int posX,
                int posY) {
  uint8_t textsize = 1;      // display->textsize;
  uint32_t textcolor = 1;    // display->textcolor;
  uint32_t textbgcolor = 0;  // display->textbgcolor;
  byte font[32];
  while (*str != 0x00) {
    // 改行処理
    if (*str == '\n') {
      // 改行
      posY += 16 * textsize;
      posX = display->getCursorX();
      str++;
      continue;
    }
    // フォント取得
    uint16_t strUTF16;
    // const_cast で const
    // を一時的に取り除いているが、根本的な解決になっていないので注意。 あくまで
    // Warning 回避してるだけ。
    str = efontUFT8toUTF16(&strUTF16, const_cast<char *>(str));
    getefontData(font, strUTF16);
    // 文字横幅
    // int width = 8 * textsize;
    int width = 16 * textsize;
    if (strUTF16 < 0x0100) {
      // 半角
      width = 8 * textsize;
    }

    // 背景塗りつぶし
    display->fillRect(posX, posY, width, 16 * textsize, textbgcolor);

    // 取得フォントの確認
    for (uint8_t row = 0; row < 16; row++) {
      word fontdata = font[row * 2] * 256 + font[row * 2 + 1];
      for (uint8_t col = 0; col < 16; col++) {
        if ((0x8000 >> col) & fontdata) {
          int drawX = posX + col * textsize;
          int drawY = posY + row * textsize;
          if (textsize == 1) {
            display->drawPixel(drawX, drawY, textcolor);
          } else {
            display->fillRect(drawX, drawY, textsize, textsize, textcolor);
          }
        }
      }
    }
    // 描画カーソルを進める
    posX += width;
    // 折返し処理
    if (display->width() <= posX) {
      posX = 0;
      posY += 16 * textsize;
    }
  }
}

void updateOLED(Adafruit_SSD1306 *display, uint8_t playCategory,
                uint8_t wearerNum, uint8_t gainStepNum) {
  USBSerial.printf("playCategory: %d, wearerNum: %d, gainStepNum: %d\n",
                   playCategory, wearerNum, gainStepNum);
  (*display).clearDisplay();
  // チャンネルの更新
  printEfont(display, menuTxt[playCategory], 0, 8);
  // 装着者の変更
  printEfont(display, wearerTxt[wearerNum], 128 - 70, 8);
  // ボリュームの更新
  printEfont(display, decibelTxt[gainStepNum], 128 - 35, 8);
  (*display).display();
  // USBSerial.printf("playCategory: %d, wearerNum: %d, gainStepNum: %d\n",
  //                  playCategory, wearerNum, gainStepNum);
}

}  // namespace displayManager
