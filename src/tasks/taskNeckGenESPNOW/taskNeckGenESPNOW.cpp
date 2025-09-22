#include "globals.h"

#if defined(NECKLACE_V2) || defined(NECKLACE_V3)
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
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1);
    if (!_isFixMode && _ampVolStep != prevAmpVolStep) {
      setAmpStepGain(_ampVolStep, true);
    }
    prevAmpVolStep = _ampVolStep;

    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t category_ID = audioManager::getCategoryID();
        uint8_t channel_ID = audioManager::getChannelID();
        audioManager::stopAudio();
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
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };

    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
#endif

#if defined(BAND_V2)
void TaskBandESPNOW() {
  uint8_t volumeLevels[CATEGORY_ID_TXT_SIZE] = {0};
  uint8_t category_ID = audioManager::getCategoryID();
  uint8_t channel_ID = audioManager::getChannelID();
  audioManager::loadVolumeLevels(volumeLevels, CATEGORY_ID_TXT_SIZE);

  while (1) {
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      int pinState = digitalRead(_SW_PIN[i]);
      if (!pinState && !_isBtnPressed[i]) {
  #if defined(BAND_V3)
        if (i == 0) {
          if (!_isFixMode) {
            if (volumeLevels[category_ID] < GAIN_STEP_TXT_SIZE - 1 - ADJ_VOL_STEP) {
              volumeLevels[category_ID] += ADJ_VOL_STEP;
            } else {
              volumeLevels[category_ID] = GAIN_STEP_TXT_SIZE - 1;
            }
            audioManager::saveVolumeLevels(volumeLevels, CATEGORY_ID_TXT_SIZE);
            setAmpStepGain(volumeLevels[category_ID], true);
          } else {
            if (category_ID < CATEGORY_ID_TXT_SIZE - 1) {
              category_ID += 1;
            } else {
              category_ID = 0;
            }
            audioManager::setCategoryID(category_ID);
            displayManager::updateOLED(&_display, category_ID, channel_ID, FIX_GAIN_STEP[category_ID]);
            setAmpStepGain(FIX_GAIN_STEP[category_ID], true);
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
            setAmpStepGain(volumeLevels[category_ID], true);
          } else {
            if (channel_ID < CHANNEL_ID_TXT_SIZE - 1) {
              channel_ID += 1;
            } else {
              channel_ID = 0;
            }
            audioManager::setChannelID(channel_ID);
            displayManager::updateOLED(&_display, category_ID, channel_ID, FIX_GAIN_STEP[category_ID]);
          }
        }
        if (i == 1) {
          if (_isFixMode) {
            _isFixMode = false;
            _leds[0] = COLOR_VOL_MODE;
            setAmpStepGain(volumeLevels[category_ID], true);
          } else {
            _isFixMode = true;
            _leds[0] = COLOR_FIX_MODE;
            setAmpStepGain(FIX_GAIN_STEP[category_ID], true);
          }
          FastLED.show();
          audioManager::setIsFixMode(_isFixMode);
        }
  #else
        if (i == 0) {
          if (channel_ID < CHANNEL_ID_TXT_SIZE - 1) {
            channel_ID += 1;
          } else {
            channel_ID = 0;
          }
          audioManager::setChannelID(channel_ID);
          displayManager::updateOLED(&_display, category_ID, channel_ID,
                                     _isFixMode ? FIX_GAIN_STEP[category_ID] : volumeLevels[category_ID]);
        } else if (i == 1) {
          if (category_ID < CATEGORY_ID_TXT_SIZE - 1) {
            category_ID += 1;
          } else {
            category_ID = 0;
          }
          audioManager::setCategoryID(category_ID);
          displayManager::updateOLED(&_display, category_ID, channel_ID,
                                     _isFixMode ? FIX_GAIN_STEP[category_ID] : volumeLevels[category_ID]);
          setAmpStepGain(_isFixMode ? FIX_GAIN_STEP[category_ID] : volumeLevels[category_ID], true);
        }
  #endif
        _isBtnPressed[i] = true;
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
#endif

void TaskUI_ESPNOW(void *args) {
#if defined(NECKLACE_V2) || defined(NECKLACE_V3)
  TaskNeckESPNOW();
#elif defined(BAND_V2) || defined(BAND_V3)
  TaskBandESPNOW();
#endif
}

// 統一インターフェース
void TaskUI_Run(void *args) { TaskUI_ESPNOW(args); }






