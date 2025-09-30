#include "globals.h"

static char volumeText[8];
static const int displayY = 10;
static const int barX = 10;
static const int barWidth = 80;
static const int barHeight = 12;
static int padx = 3;

static const int ADC_FILTER_SIZE = 4;
static int adcBuffer[ADC_FILTER_SIZE];
static int adcIndex = 0;
static int adcSum = 0;
static int stableCounter = 0;
static const int STABLE_COUNT = 1;

int filterADCValue(int newValue) {
  adcSum -= adcBuffer[adcIndex];
  adcBuffer[adcIndex] = newValue;
  adcSum += newValue;
  adcIndex = (adcIndex + 1) % ADC_FILTER_SIZE;
  return adcSum / ADC_FILTER_SIZE;
}

void initADCFilter(int initialValue) {
  for (int i = 0; i < ADC_FILTER_SIZE; i++) { adcBuffer[i] = initialValue; }
  adcSum = initialValue * ADC_FILTER_SIZE; adcIndex = 0;
}

void updateJudoVolumeDisplay(int ampVolStep) {
  _display.clearDisplay();
  _display.setFont();
  _display.setTextSize(2);
  _display.setTextColor(SSD1306_WHITE);
  snprintf(volumeText, sizeof(volumeText), "%d", ampVolStep);
  _display.setCursor(barX + barWidth + 8, displayY-1);
  _display.print(volumeText);
  _display.drawRect(barX, displayY, barWidth, barHeight, SSD1306_WHITE);
  int fillWidth = map(ampVolStep, 0, 23, 0, barWidth - 2);
  if (fillWidth > 0) { _display.fillRect(barX + 1, displayY + 1, fillWidth, barHeight - 2, SSD1306_WHITE); }
  _display.display();
  _lastDisplayUpdate = millis();
  USBSerial.printf("JUDO0806 Mode - Volume: %d\n", ampVolStep);
}

void TaskJudoNeck() {
  audioManager::setCategoryID(2);
  _isFixMode = false; audioManager::setIsFixMode(false);
  _currAIN = analogRead(AIN_VIBVOL_PIN); initADCFilter(_currAIN);
  if (DISP_ROT == 90) { _ampVolStep = map(_currAIN, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0); }
  else { _ampVolStep = map(_currAIN, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1); }
  setAmpStepGain(_ampVolStep, false); updateJudoVolumeDisplay(_ampVolStep);
  int prevVolStep = _ampVolStep;
  while (1) {
    int rawADC = analogRead(AIN_VIBVOL_PIN);
    int filteredADC = filterADCValue(rawADC);
    int newVolStep;
    if (DISP_ROT == 90) { newVolStep = map(filteredADC, 0, 4095, GAIN_STEP_TXT_SIZE - 1, 0); }
    else { newVolStep = map(filteredADC, 0, 4095, 0, GAIN_STEP_TXT_SIZE - 1); }
    if (newVolStep == prevVolStep) { stableCounter++; }
    else { stableCounter = 0; prevVolStep = newVolStep; }
    if (stableCounter >= STABLE_COUNT && newVolStep != _ampVolStep) {
      _ampVolStep = newVolStep; setAmpStepGain(_ampVolStep, false); updateJudoVolumeDisplay(_ampVolStep); stableCounter = 0;
    }
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t channel_ID = audioManager::getChannelID(); audioManager::stopAudio();
        if (i == 1 && channel_ID < CHANNEL_ID_TXT_SIZE - 1) { channel_ID += 1; }
        else if (i == 0 && channel_ID > 0) { channel_ID -= 1; }
        _isBtnPressed[i] = true;
        if (i == 0 || i == 1) { audioManager::setChannelID(channel_ID); }
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) { _isBtnPressed[i] = false; }
    }
    _leds[0] = COLOR_VOL_MODE; FastLED.show(); vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}

void TaskUI_JUDO0806(void *args) {
  TaskJudoNeck();
}

