#include <Adafruit_SSD1306.h>

#ifndef DISPLAY_MANAGER_H
  #define DISPLAY_MANAGER_H
namespace displayManager {
void initOLED(Adafruit_SSD1306 *display, int rot, const int textsize,
              const int playPos[2], const int channelPos[2],
              const int gainPos[2]);
void setTitle(const char **category, int m_size, const char **channel,
              int w_size, const char **gainstep, int d_size);
void updateOLED(Adafruit_SSD1306 *display, uint8_t categoryNum,
                uint8_t channelNum, uint8_t gainStepNum);
void setGain(uint8_t G_SEL_A, uint8_t G_SEL_B, uint8_t val);
void printEfont(Adafruit_SSD1306 *display, const char *str, int posX, int posY);
void drawBatteryMeter5(Adafruit_SSD1306 *display, int x, int y, int barW, int barH,
                      int gap, int levels, bool blinkOn, bool blinkOne,
                      bool withLabel);
}  // namespace displayManager

#endif