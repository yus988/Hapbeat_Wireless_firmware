#include "globals.h"

static void drawTopRowFromParams(Adafruit_SSD1306 *display,
                                 uint8_t cat, uint8_t ch, uint8_t volStep) {
  display->fillRect(0, 0, display->width(), 16, 0);
  const char *catLabel = CATEGORY_ID_TXT[cat];
  const char *chLabel = CHANNEL_ID_TXT[ch];
  const char *volLabel = GAIN_STEP_TXT[volStep];

  // 左端を基準にし、Cat, CH, Vol の間隔を等間隔に配置
  int w = display->width();
  int catW = (int)strlen(catLabel) * 8;
  int chW  = (int)strlen(chLabel) * 8;
  int volW = (int)strlen(volLabel) * 8;
  int gap = (w - (catW + chW + volW));
  if (gap < 0) gap = 0;
  gap /= 2;
  int x1 = 0;
  int x2 = x1 + catW + gap;
  int x3 = x2 + chW + gap;

  displayManager::printEfont(display, catLabel, x1, 0);
  displayManager::printEfont(display, chLabel,  x2, 0);
  displayManager::printEfont(display, volLabel, x3, 0);
}

static void drawBatterySquares(Adafruit_SSD1306 *display, int levels) {
  display->fillRect(0, 16, display->width(), 16, 0);
  const int boxSize = 10;
  // 左側に "BAT" ラベル表示（位置は固定: x=0, y=16）
  const char *bat = "BAT";
  displayManager::printEfont(display, bat, 0, 16);
  int batW = (int)strlen(bat) * 8;
  const int padding = 6;

  // 残り領域（BAT分を除いた右側）で、左右マージンも含めて等間隔配置
  int usable = display->width() - (batW + padding);
  if (usable < 0) usable = 0;
  // N個の四角（幅=boxSize）と、N+1個のgapで均等割
  const int N = 5;
  int gap = (usable - N * boxSize) / (N + 1);
  if (gap < 0) gap = 0;
  int startX = batW + padding + gap;  // 左マージン=gap
  // 下辺を BAT の下辺（y=16, 高さ相当=12px）に合わせる
  const int targetBottom = 16 + 12 + 1;
  const int y = targetBottom - boxSize;
  for (int i = 0; i < N; i++) {
    int x = startX + i * (boxSize + gap);
    if (i < levels) {
      display->fillRect(x, y, boxSize, boxSize, SSD1306_WHITE);
    } else {
      display->drawRect(x, y, boxSize, boxSize, SSD1306_WHITE);
    }
  }
  display->display();
}

void TaskBandESPNOW(void *args) {
  // 既存の Band V3 実装に合わせた新UI版
  // 元ソース: src/taskBandESPNOW/taskBandESPNOW.cpp
  // そのまま移植

  uint8_t volumeLevels[CATEGORY_ID_TXT_SIZE] = {0};
  uint8_t category_ID = audioManager::getCategoryID();
  uint8_t channel_ID = audioManager::getChannelID();
  audioManager::loadVolumeLevels(volumeLevels, CATEGORY_ID_TXT_SIZE);
  
  uint8_t prevCat = category_ID;
  uint8_t prevCh = channel_ID;
  uint8_t prevVolStep = _isFixMode ? FIX_GAIN_STEP[category_ID] : volumeLevels[category_ID];
  int prevLevels = -1;
  bool blinkOne = false;           // soc<1% の時 true
  bool blinkLedOn = true;          // LED点滅用
  bool needRedrawTop = true;
  bool needRedrawBat = true;
  unsigned long lastBlinkLed = 0;
  unsigned long lastBatSample = 0;

  while (1) {
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      int pinState = digitalRead(_SW_PIN[i]);
      if (!pinState && !_isBtnPressed[i]) {
        if (i == 0) {
          if (!_isFixMode) {
            if (volumeLevels[category_ID] < GAIN_STEP_TXT_SIZE - 1 - ADJ_VOL_STEP) {
              volumeLevels[category_ID] += ADJ_VOL_STEP;
            } else {
              volumeLevels[category_ID] = GAIN_STEP_TXT_SIZE - 1;
            }
            audioManager::saveVolumeLevels(volumeLevels, CATEGORY_ID_TXT_SIZE);
            setAmpStepGain(volumeLevels[category_ID], false);
            needRedrawTop = true; needRedrawBat = true;
          } else {
            if (category_ID < CATEGORY_ID_TXT_SIZE - 1) category_ID += 1; else category_ID = 0;
            audioManager::setCategoryID(category_ID);
            setAmpStepGain(FIX_GAIN_STEP[category_ID], false);
            needRedrawTop = true; needRedrawBat = true;
          }
        }
        if (i == 2) {
          if (!_isFixMode) {
            if (volumeLevels[category_ID] == GAIN_STEP_TXT_SIZE - 1 && ADJ_VOL_STEP != 1) {
              volumeLevels[category_ID] -= ADJ_VOL_STEP - 1;
            } else if (volumeLevels[category_ID] > ADJ_VOL_STEP - 1) {
              volumeLevels[category_ID] -= ADJ_VOL_STEP;
            } else {
              volumeLevels[category_ID] = 0;
            }
            audioManager::saveVolumeLevels(volumeLevels, CATEGORY_ID_TXT_SIZE);
            setAmpStepGain(volumeLevels[category_ID], false);
            needRedrawTop = true; needRedrawBat = true;
          } else {
            if (channel_ID < CHANNEL_ID_TXT_SIZE - 1) channel_ID += 1; else channel_ID = 0;
            audioManager::setChannelID(channel_ID);
            setAmpStepGain(FIX_GAIN_STEP[category_ID], false);
            needRedrawTop = true; needRedrawBat = true;
          }
        }
        if (i == 1) {
          if (_isFixMode) {
            _isFixMode = false; _leds[0] = COLOR_VOL_MODE; setAmpStepGain(volumeLevels[category_ID], false);
          } else {
            _isFixMode = true; _leds[0] = COLOR_FIX_MODE; setAmpStepGain(FIX_GAIN_STEP[category_ID], false);
          }
          FastLED.show();
          audioManager::setIsFixMode(_isFixMode);
          needRedrawTop = true; needRedrawBat = true;
        }
        _isBtnPressed[i] = true;
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    }

    unsigned long now = millis();
    if (now - lastBatSample >= 1000) {
      lastBatSample = now;
      int levels;
      bool newBlinkOne;
#ifdef BAT_DEBUG
      static uint8_t dbgStage = 0;           // 0..5 (5=LED blink stage)
      static unsigned long dbgLast = 0;
      if (now - dbgLast >= 3000) { dbgLast = now; dbgStage = (dbgStage + 1) % 6; }
      levels = (dbgStage < 5) ? (5 - dbgStage) : 1;
      newBlinkOne = (dbgStage == 5);
#else
      uint16_t socPct = lipo.soc();
      if (socPct == 0) { delay(20); socPct = lipo.soc(); }
      if (lipo.voltage() >= 4000) socPct = 100;
      levels = (socPct >= 100) ? 5 : (socPct >= 80 ? 5 : socPct >= 60 ? 4 : socPct >= 40 ? 3 : socPct >= 20 ? 2 : socPct > 0 ? 1 : 0);
      newBlinkOne = (socPct < 1);
#endif
      if (levels != prevLevels || newBlinkOne != blinkOne) {
        bool wasBlinking = blinkOne;
        prevLevels = levels;
        blinkOne = newBlinkOne;
        if (wasBlinking && !blinkOne) { // 0%相当->復帰でLEDを通常色へ
          _leds[0] = _isFixMode ? COLOR_FIX_MODE : COLOR_VOL_MODE;
          FastLED.show();
        }
        needRedrawBat = true;
      }
    }
    if (blinkOne && (now - lastBlinkLed >= 300)) {
      lastBlinkLed = now;
      blinkLedOn = !blinkLedOn;
      _leds[0] = blinkLedOn ? CRGB(10, 0, 0) : (_isFixMode ? COLOR_FIX_MODE : COLOR_VOL_MODE);
      FastLED.show();
    }

    uint8_t curVol = _isFixMode ? FIX_GAIN_STEP[category_ID] : volumeLevels[category_ID];
    if (needRedrawTop || category_ID != prevCat || channel_ID != prevCh || curVol != prevVolStep) {
      drawTopRowFromParams(&_display, category_ID, channel_ID, curVol);
      prevCat = category_ID; prevCh = channel_ID; prevVolStep = curVol;
      needRedrawBat = true; needRedrawTop = false;
    }
    if (needRedrawBat) {
      drawBatterySquares(&_display, prevLevels);
      needRedrawBat = false;
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}






