#include "globals.h"

#if defined(NECKLACE_V2)
void TaskNeckESPNOW() {
  if (_isFixMode) {
    setFixGain();
  } else {
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
    setAmpStepGain(_ampVolStep, true);
  }
  uint8_t prevAmpVolStep = 0;
  while (1) {
    // USBSerial.print("Digipot wiper: ");
    // USBSerial.print(_digipot.getWiperValue(), DEC);
    // USBSerial.println('%');
    // BQ27220_Cmd::printBatteryStats();
    // control pam8003 volume
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
    // uint8_t _ampVolStep = 0;
    if (!_isFixMode && _ampVolStep != prevAmpVolStep) {
      setAmpStepGain(_ampVolStep, true);
    }
    prevAmpVolStep = _ampVolStep;

    // ボタン操作
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t category_ID = audioManager::getCategoryID();
        uint8_t channel_ID = audioManager::getChannelID();
        audioManager::stopAudio();
        // 各ボタン毎の操作 0,1 = 上下, 2,3 = 左右, 4 = 右下
        if (i == 1 && channel_ID < CHANNEL_ID_TXT_SIZE - 1) {
          channel_ID += 1;
        } else if (i == 0 && channel_ID > 0) {
          channel_ID -= 1;
        } else if (i == 2 && (category_ID < CATEGORY_ID_TXT_SIZE - 1)) {
          category_ID += 1;
        } else if (i == 3 && category_ID > 0) {
          category_ID -= 1;
        } else if (i == 4) {
          if (_isFixMode) {
            _isFixMode = false;
            _leds[0] = COLOR_VOL_MODE;
            setAmpStepGain(_ampVolStep, true);
          } else {
            _isFixMode = true;
            _leds[0] = COLOR_FIX_MODE;
            setFixGain();
          }
          FastLED.show();
          audioManager::setIsFixMode(_isFixMode);
        }
        _isBtnPressed[i] = true;
        if (i != 4) {
          int tstep = (_isFixMode) ? FIX_GAIN_STEP[category_ID] : _ampVolStep;
          displayManager::updateOLED(&_display, category_ID, channel_ID, tstep);
          audioManager::setCategoryID(category_ID);
          audioManager::setChannelID(channel_ID);
          setAmpStepGain(tstep, true);
        }
        // vTaskDelay(1 / portTICK_PERIOD_MS);
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };

    // loop delay
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
#endif

#if defined(BAND_V2)
void TaskBandESPNOW() {
  while (1) {
    // デバッグ用、電池残量表示
    // BQ27220_Cmd::printBatteryStats();
    // ボタン操作
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t category_ID = audioManager::getCategoryID();
        uint8_t channel_ID = audioManager::getChannelID();
        audioManager::stopAudio();

  #if defined(BAND_V3)
        // 上から、0, 2, 1 の並びであることに注意
        if (i == 0) {
          if (category_ID < CATEGORY_ID_TXT_SIZE - 1) {
            category_ID += 1;
          } else {
            category_ID = 0;
          }
        }
        if (i == 2) {
          if (channel_ID < CHANNEL_ID_TXT_SIZE - 1) {
            channel_ID += 1;
          } else {
            channel_ID = 0;
          }
        }

  #else
          // if (i == 2 && channel_ID < CHANNEL_ID_TXT_SIZE - 1) {
          //   channel_ID += 1;
          // } else if (i == 0 && channel_ID > 0) {
          //   channel_ID -= 1;
          // } else if (i == 1) {
          //   if (category_ID < CATEGORY_ID_TXT_SIZE - 1) {
          //     category_ID += 1;
          //   } else {
          //     category_ID = 0;
          //   }
          // }
  #endif
        _isBtnPressed[i] = true;
        displayManager::updateOLED(&_display, category_ID, channel_ID,
                                   FIX_GAIN_STEP[category_ID]);
        audioManager::setCategoryID(category_ID);
        audioManager::setChannelID(channel_ID);
        setAmpStepGain(FIX_GAIN_STEP[category_ID], true);
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };
    // loop delay
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
#endif

// main.cpp に出力する。用途に応じて適応するものを選択。
void TaskUI_ESPNOW(void *args) {
#if defined(NECKLACE_V2)
  TaskNeckESPNOW();
#elif defined(BAND_V2)
  TaskBandESPNOW();
#endif
}