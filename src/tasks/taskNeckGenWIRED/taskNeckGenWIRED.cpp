#include "globals.h"
#include <Fonts/FreeSansBold9pt7b.h>

#if defined(NECKLACE_V2) || defined(NECKLACE_V3)
#if defined(WIRED)

static char percentText[8];
static const int barX = 2;
static const int barY = 15;
static const int barWidth = 122;
static const int barHeight = 12;
static bool isMuted = false;
static bool reverseVolumeDirection = true;
static int padx = 3;
static int pady = 2;

void updateVolumeDisplay(int ampVolStep) {
  int volumePercent = map(ampVolStep, 0, 23, 0, 100);
  _display.ssd1306_command(SSD1306_DISPLAYON);
  _display.clearDisplay();
  _display.setFont();
  _display.setTextSize(1);
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(padx, pady);
  _display.print("Volume");
  snprintf(percentText, sizeof(percentText), "%d%%", volumePercent);
  int percentWidth = strlen(percentText) * 6;
  _display.setCursor(128 - percentWidth - padx, pady);
  _display.print(percentText);
  _display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);
  int fillWidth = map(volumePercent, 0, 100, 0, barWidth - 4);
  if (fillWidth > 0) { _display.fillRect(barX + 2, barY + 2, fillWidth, barHeight - 4, SSD1306_WHITE); }
  _display.display();
  _lastDisplayUpdate = millis();
  USBSerial.printf("WIRED Mode - Volume: %d%% (Step: %d)\n", volumePercent, ampVolStep);
}

void updateMuteDisplay() {
  _display.ssd1306_command(SSD1306_DISPLAYON);
  _display.clearDisplay();
  _display.setFont(&FreeSansBold9pt7b);
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(9, 15);
  _display.print("MUTE: press");
  _display.setCursor(5, 30);
  _display.print("btn to unmute");
  _display.setFont();
  _display.display();
  _lastDisplayUpdate = millis();
  USBSerial.println("WIRED Mode - MUTED");
}

void TaskWiredNeck() {
  _currAIN = analogRead(AIN_VIBVOL_PIN);
  if (reverseVolumeDirection) {
    _ampVolStep = map(_currAIN, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0);
  } else {
    _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
  }
  uint8_t prevAmpVolStep = 0;

  while (1) {
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    if (reverseVolumeDirection) {
      _ampVolStep = map(_currAIN, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0);
    } else {
      _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
    }
    if (!isMuted) {
      if (_ampVolStep != prevAmpVolStep) {
        setAmpStepGain(_ampVolStep, false);
        updateVolumeDisplay(_ampVolStep);
      }
    }
    prevAmpVolStep = _ampVolStep;

    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        _isBtnPressed[i] = true;
        if (ENABLE_WIRED_MUTE_FUNCTION) {
          isMuted = !isMuted;
          if (isMuted) {
            _isFixMode = true; audioManager::setIsFixMode(true);
            setAmpStepGain(0, false); updateMuteDisplay(); _leds[0] = COLOR_FIX_MODE;
          } else {
            _isFixMode = false; audioManager::setIsFixMode(false);
            setAmpStepGain(_ampVolStep, false); updateVolumeDisplay(_ampVolStep); _leds[0] = COLOR_VOL_MODE;
          }
          FastLED.show();
        }
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) { _isBtnPressed[i] = false; }
    }

    if (!isMuted) { _leds[0] = COLOR_VOL_MODE; FastLED.show(); }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

#endif
#endif

void TaskUI_WIRED(void *args) {
#if defined(NECKLACE_V2) || defined(NECKLACE_V3)
#if defined(WIRED)
  TaskWiredNeck();
#endif
#endif
}

// 統一インターフェース
void TaskUI_Run(void *args) { TaskUI_WIRED(args); }






