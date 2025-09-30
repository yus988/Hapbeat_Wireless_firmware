#include "globals.h"

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

void TaskUI_ESPNOW(void *args) {
#if defined(NECKLACE_V2) || defined(NECKLACE_V3)
  TaskNeckESPNOW();
#endif
}

// 統一インターフェース
void TaskUI_Run(void *args) { TaskUI_ESPNOW(args); }






