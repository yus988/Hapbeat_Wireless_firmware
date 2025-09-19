#include "globals.h"
#include <cstring>

// 上段のみ（y<16）を再描画。Cat 左, CH 中央, Vol 右
static void drawTopRow(Adafruit_SSD1306 *display,
                       uint8_t cat, uint8_t ch, uint8_t volStep) {
  display->fillRect(0, 0, display->width(), 16, 0);
  char catStr[12]; snprintf(catStr, sizeof(catStr), "Cat %u", cat);
  displayManager::printEfont(display, catStr, 0, 0);
  char chStr[12]; snprintf(chStr, sizeof(chStr), "CH %u", ch);
  int chW = (int)strlen(chStr) * 8;
  int chX = (display->width() - chW) / 2; if (chX < 0) chX = 0;
  displayManager::printEfont(display, chStr, chX, 0);
  char volStr[16]; snprintf(volStr, sizeof(volStr), "Vol %u", volStep);
  int volW = (int)strlen(volStr) * 8; int volX = display->width() - volW; if (volX < 0) volX = 0;
  displayManager::printEfont(display, volStr, volX, 0);
}

// 下段のみ（y>=16）を再描画する。上段は updateOLED に任せる。
static void drawBatteryBottom(Adafruit_SSD1306 *display,
                              int levels, bool blinkOn, bool blinkOne) {
  display->fillRect(0, 16, display->width(), 16, 0);
  displayManager::drawBatteryMeter5(display, 36, 16, 10, 12, 6,
                                    levels, blinkOn, blinkOne, true);
  display->display();
}

void TaskBandESPNOW(void *args) {
  // カテゴリごとのボリューム設定
  uint8_t volumeLevels[CATEGORY_ID_TXT_SIZE] = {0};
  uint8_t category_ID = audioManager::getCategoryID();
  uint8_t channel_ID = audioManager::getChannelID();
  audioManager::loadVolumeLevels(volumeLevels, CATEGORY_ID_TXT_SIZE);
  
  // 状態保持（前回表示値）
  uint8_t prevCat = category_ID;
  uint8_t prevCh = channel_ID;
  uint8_t prevVolStep = _isFixMode ? FIX_GAIN_STEP[category_ID] : volumeLevels[category_ID];
  int prevLevels = -1;
  bool blinkOne = false;
  bool blinkOn = true;
  bool needRedrawTop = true;
  bool needRedrawBat = true;
  unsigned long lastBlink = 0;
  unsigned long lastBatSample = 0;

  while (1) {
    // ボタン操作
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

    // バッテリーサンプリング（1秒間隔）と点滅タイミング（500ms）
    unsigned long now = millis();
    if (now - lastBatSample >= 1000) {
      lastBatSample = now;
      int levels;
      bool newBlinkOne;
#ifdef BAT_DEBUG
      static uint8_t dbgStage = 0;           // 0..5 (5=blink)
      static unsigned long dbgLast = 0;
      if (now - dbgLast >= 3000) { dbgLast = now; dbgStage = (dbgStage + 1) % 6; }
      if (dbgStage < 5) { levels = 5 - dbgStage; newBlinkOne = false; }
      else { levels = 1; newBlinkOne = true; }
#else
      uint16_t socPct = lipo.soc();
      if (socPct == 0) { delay(20); socPct = lipo.soc(); }
      if (lipo.voltage() >= 4000) socPct = 100;
      levels = (socPct >= 100) ? 5 : (socPct >= 80 ? 5 : socPct >= 60 ? 4 : socPct >= 40 ? 3 : socPct >= 20 ? 2 : socPct > 0 ? 1 : 0);
      newBlinkOne = (socPct < 1);
#endif
      if (levels != prevLevels || newBlinkOne != blinkOne) {
        prevLevels = levels;
        blinkOne = newBlinkOne;
        needRedrawBat = true;
      }
    }
    if (blinkOne && (now - lastBlink >= 500)) {
      lastBlink = now;
      blinkOn = !blinkOn;
      needRedrawBat = true;
    }

    // 画面更新（必要な時のみ）
    uint8_t curVol = _isFixMode ? FIX_GAIN_STEP[category_ID] : volumeLevels[category_ID];
    if (needRedrawTop || category_ID != prevCat || channel_ID != prevCh || curVol != prevVolStep) {
      drawTopRow(&_display, category_ID, channel_ID, curVol);
      prevCat = category_ID; prevCh = channel_ID; prevVolStep = curVol;
      needRedrawBat = true; needRedrawTop = false;
    }
    if (needRedrawBat) {
      // 現在のlevelsを毎回計算（消える現象対策で再確定）
      uint16_t socPct = lipo.soc(); if (socPct == 0) { delay(20); socPct = lipo.soc(); }
      if (lipo.voltage() >= 4000) socPct = 100;
      int levelsNow = (socPct >= 100) ? 5 : (socPct >= 80 ? 5 : socPct >= 60 ? 4 : socPct >= 40 ? 3 : socPct >= 20 ? 2 : socPct > 0 ? 1 : 0);
      prevLevels = levelsNow;
      drawBatteryBottom(&_display, prevLevels, blinkOn, blinkOne);
      needRedrawBat = false;
    }

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}


