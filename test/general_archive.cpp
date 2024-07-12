// pinAssign.h
#if defined(GENERAL)
  // pins related to audio
  #define I2S_BCLK_PIN 39
  #define I2S_DOUT_PIN 40
  #define I2S_LRCK_PIN 42
  #define SD_MODE_PIN 16  // for MAX98357A
  #define G_SEL_A_PIN 33
  #define G_SEL_B_PIN 34
  // Button pins
  #define SW1_PIN 4
  #define SW2_PIN 21
  #define SW3_PIN 48
  // LED
  #define LED_PIN 45
#endif

// main.cpp before setup
#if defined(GENERAL)
  #define MAX_GAIN_NUM 3
CRGB _leds[4];
int _SW_PIN[] = {SW1_PIN, SW2_PIN, SW3_PIN};
bool _isBtnPressed[] = {false, false, false};  // control state of being pressed
uint8_t gainNum;
// 電源OFFしても覚えておきたい
uint8_t devicePos = 0;
uint8_t _wearerId = 0;
uint8_t ledPower = 0;
uint8_t ledPowerCoef = 4;
uint8_t ledPowerConst = 3;
TaskHandle_t thp[2];
#endif

#ifdef GENERAL
void TaskUI(void *args) {
  while (1) {
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t playCategoryNum = audioManager::getPlayCategory();
        uint8_t gainNum = audioManager::getGain();
        uint8_t wearId = audioManager::getWearerId();
        audioManager::stopAudio();
        // 各ボタン毎の操作 0=wearer, 1=pos, 2=gain
        if (i == 0) {
          if (playCategoryNum < 1) {
            playCategoryNum += 1;
          } else {
            playCategoryNum = 0;
          }
          audioManager::setPlayCategory(playCategoryNum);
          USBSerial.println(playCategoryNum);
          // if (wearId < 3) {
          //   wearId += 1;
          // } else {
          //   wearId = 0;
          // }
        } else if (i == 2) {
          devicePos += 1;
          // audioManager::setDevicePos(devicePos);
        } else if (i == 1) {
          gainNum += 1;
          if (gainNum > MAX_GAIN_NUM) {
            gainNum = 0;
          }
          audioManager::setGain(gainNum, G_SEL_A_PIN, G_SEL_B_PIN);
          ledPower = gainNum * ledPowerCoef + ledPowerConst;
        }
        for (int j = 0; j < 4; j++) {
          // if (j == wearId) {
          if (j == playCategoryNum) {
            _leds[j].setRGB(ledPower, ledPower, ledPower);
          } else {
            _leds[j].setRGB(0, 0, 0);
          }
        }
        FastLED.show();
        _isBtnPressed[i] = true;
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) {
        _isBtnPressed[i] = false;
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

// 本来は電流測定用だが、MQTTループを回してみる
void TaskCurrent(void *args) {
  while (1) {
    MQTT_manager::loopMQTTclient();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

// MQTT受信用、General v2 からはネックレスと共用にできるはず
void statusCallback(const char *status) { USBSerial.println(status); }

#endif

// main.cpp setup
#ifdef GENERAL
  pinMode(G_SEL_B_PIN, OUTPUT);
  pinMode(G_SEL_A_PIN, OUTPUT);
  pinMode(SW1_PIN, INPUT);
  pinMode(SW2_PIN, INPUT);
  pinMode(SW3_PIN, INPUT);
  // Show LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(_leds, 4);
  for (int i = 0; i < 4; i++) {
    _leds[i] = CRGB::Black;
  }
  // MAX初期設定
  pinMode(SD_MODE_PIN, OUTPUT);
  digitalWrite(SD_MODE_PIN, HIGH);
  gainNum = 3;
  audioManager::setGain(2, G_SEL_A_PIN, G_SEL_B_PIN);
  audioManager::setDevicePos(5);
  audioManager::setPlayCategory(0);
  uint8_t wearId = 0;
  audioManager::setWearerId(wearId);
  ledPower = gainNum * ledPowerCoef + ledPowerConst;
  for (int j = 0; j < 4; j++) {
    if (j == wearId) {
      _leds[j].setRGB(ledPower, ledPower, ledPower);
    } else {
      _leds[j].setRGB(0, 0, 0);
    }
  }
  FastLED.show();
#endif

// audio manager.cpp initAudioOut
#ifdef GENERAL
  // MOSFET直結のD級アンプにI2Sを入れる場合は、始めに再生するすることで I2S
  // clockを発生させる
  playAudio(0, 0);
#endif

// audio manager.cpp setGain
#ifdef GENERAL 
  // using RS2255XN and MAX98357
  switch (val) {
    case 0:  // 6dB
      digitalWrite(G_SEL_B, LOW);
      digitalWrite(G_SEL_A, LOW);
      break;
    case 1:  // 9dB
      digitalWrite(G_SEL_B, LOW);
      digitalWrite(G_SEL_A, HIGH);
      break;
    case 2:  // 12dB
      digitalWrite(G_SEL_B, HIGH);
      digitalWrite(G_SEL_A, LOW);
      break;
    case 3:  // 15dB
      digitalWrite(G_SEL_B, HIGH);
      digitalWrite(G_SEL_A, HIGH);
      break;
    default:
      break;
  }
#endif