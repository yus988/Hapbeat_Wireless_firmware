#include <Adafruit_SSD1306.h>

#ifndef DISPLAY_MANAGER_H
  #define DISPLAY_MANAGER_H
namespace displayManager {
void initOLED(Adafruit_SSD1306 *display, int rot);
void setTitle(const char **menu, int m_size, const char **wearer, int w_size,
              const char **decibel, int d_size);
void updateOLED(Adafruit_SSD1306 *display, uint8_t playCategory,
                uint8_t wearerNum, uint8_t gainStepNum);
void setGain(uint8_t G_SEL_A, uint8_t G_SEL_B, uint8_t val);
void printEfont(Adafruit_SSD1306 *display, const char *str, int posX, int posY);
}  // namespace displayManager

#endif