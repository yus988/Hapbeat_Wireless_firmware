#include <Arduino.h>

#include <iostream>
// ディスプレイ関連
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#include <Wire.h>
// 自作ライブラリ
#include <BQ27220.h>
#include <audioManager.h>
#include <displayManager.h>

#ifdef ESPNOW
  #include <espnow_manager.h>
#elif MQTT
  #include <MQTT_manager.h>
#endif

#include "adjustmentParams.h"
#include "pinAssign.h"

/////////////////////////// 変数および汎用関数の宣言
/////////////////////////////////////////
#if defined(NECKLACE) || defined(NECKLACE_V_1_3) || defined(GENERAL_V2)
// _display params
const int DISP_ROT = 0;
// const int DISP_ROT = 90; // 上下逆
const int SCREEN_WIDTH = 128;  // OLED _display width, in pixels
const int SCREEN_HEIGHT = 32;  // OLED _display height, in pixels

// 定数の定義
const float SHUNT_RESISTANCE = 0.01;  // シャント抵抗 (オーム)
const float INA_GAIN = 50.0;          // INA180A2IDBVRのゲイン (V/V)
const int ADC_MAX = 4095;             // ADCの分解能
const float V_REF = 3.3;              // アナログ基準電圧 (V)
const int BATTERY_CAPACITY = 3000;    // バッテリー容量 (mAh)

  #if defined(NECKLACE_V_1_3)
TaskHandle_t thp[3];
  #else
TaskHandle_t thp[2];
  #endif

// LED
CRGB _leds[1];
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                          OLED_DC_PIN, OLED_RESET_PIN, CS_PIN);

  #if defined(NECKLACE) || defined(NECKLACE_V_1_3)
// see pam8003 datasheet p.7

int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN, SW3_SEL_P_PIN, SW4_SEL_N_PIN,
                 SW5_ENTER_PIN};
bool _isBtnPressed[] = {false, false, false, false, false};
  #endif

  #if defined(GENERAL_V2)
// see pam8003 datasheet p.7

int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN};
bool _isBtnPressed[] = {false, false};
  #endif

bool _isFixMode;

  #if defined(NECKLACE) || defined(NECKLACE_V_1_3)
// volume related variables
int _prevAIN = 0;
int _currAIN = 0;
bool _disableVolumeControl = false;
uint8_t _ampVolStep;
  #endif
  #if defined(NECKLACE_V_1_3)
// 電流を計算するための関数
float calculateCurrent(int adc_value) {
  // ADCの値を電圧に変換
  float voltage = (adc_value * V_REF) / ADC_MAX;
  // 電圧を電流に変換
  float current = voltage / (INA_GAIN * SHUNT_RESISTANCE);
  return current;
}
  #endif

void statusCallback(const char *status) {
  _display.clearDisplay();
  _display.setCursor(0, 0);
  displayManager::printEfont(&_display, status, 0, 0);
  _display.display();
}

// コールバック関数の定義
void messageReceived(char *topic, byte *payload, unsigned int length) {
  USBSerial.print("Message arrived in topic: ");
  USBSerial.println(topic);
  USBSerial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    USBSerial.print((char)payload[i]);
  }
  USBSerial.println();
}

// 所定の値に固定する。
void setFixGain(bool updateOLED = true) {
  // 15dBにあたるステップ数0--63をanalogWrite0--255に変換する
  analogWrite(
      AOUT_VIBVOL_PIN,
      map(_fixGainStep[audioManager::getPlayCategory()], 0, 63, 0, 255));
  if (updateOLED) {
    displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                               audioManager::getWearerId(),
                               _fixGainStep[audioManager::getPlayCategory()]);
  }
}
#endif

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

////////////////////////////////// define tasks ////////////////////////////////
// 優先度は最低にする
void TaskAudio(void *args) {
  while (1) {
    audioManager::playAudioInLoop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

#if defined(NECKLACE) || defined(NECKLACE_V_1_3)
// PAMの電圧を下げる
void setAmpStepGain(int step, bool updateOLED = true) {
  int volume = map(_currAIN, 0, 4095, 0, 255);
  analogWrite(AOUT_VIBVOL_PIN, volume);
  // ディスプレイにdB表示用のステップ数変換
  if (updateOLED) {
    displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                               audioManager::getWearerId(), step);
  }
}
void TaskCurrent(void *args) {
  int adc_value;
  float current;
  int shutdownCounter[2] = {0, 0};
  int restoreCounter = 0;  // カウンタの初期化
  uint8_t modifiedStep;
  while (1) {
    // ADC値の読み取り
    adc_value = analogRead(BAT_CURRENT_PIN);
    // 電流値の計算
    current = calculateCurrent(adc_value);
    if (current > current_thresholds[0]) {
      shutdownCounter[0]++;
      if (current > current_thresholds[1]) {
        shutdownCounter[1]++;
      }
      if (shutdownCounter[0] >= shutdownCycles[0] ||
          shutdownCounter[1] >= shutdownCycles[1]) {
        // 電流値が閾値を超えた場合にアンプをミュート
        digitalWrite(EN_VIBAMP_PIN, LOW);
        _disableVolumeControl = true;  // 音量操作を無効化
        _leds[0] = _colorDangerMode;
        FastLED.show();
        restoreCounter = 0;  // カウンタリセット
      }
    } else {
      if (restoreCounter >= restoreCycles) {
        _disableVolumeControl = false;  // 音量操作を有効化
        digitalWrite(EN_VIBAMP_PIN, HIGH);
        if (_isFixMode) {
          setFixGain(false);
          _leds[0] = _colorFixMode;
        } else {
          setAmpStepGain(_ampVolStep, false);
          _leds[0] = _colorVolumeMode;
        }
        FastLED.show();
        shutdownCounter[0] = 0;  // カウンタリセット
        shutdownCounter[1] = 1;
      } else {
        restoreCounter++;  // カウンタ増加
      }
    }
    // 電流値をシリアルモニタに出力
    // USBSerial.printf("Measured Current: %.6f A\n", current);
    // 100ms待機
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void TaskUI(void *args) {
  if (_isFixMode) {
    setFixGain();
  }
  while (1) {
    // BQ27220_Cmd::printBatteryStats();
    // control pam8003 volume
    _currAIN = analogRead(AIN_VIBVOL_PIN);
    _ampVolStep = map(_currAIN, 0, 4095, 0, 63);
    // uint8_t _ampVolStep = 0;
    if (!_isFixMode && !_disableVolumeControl &&
        abs(_currAIN - _prevAIN) > volumeThreshold) {
      setAmpStepGain(_ampVolStep);
    }
    _prevAIN = _currAIN;

    // ボタン操作
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t playCategoryNum = audioManager::getPlayCategory();
        uint8_t wearId = audioManager::getWearerId();
        audioManager::stopAudio();
        // 各ボタン毎の操作 0,1 = 上下, 2,3 = 左右, 4 = 右下
        if (i == 1 &&
            wearId < sizeof(_wearerIdTxt) / sizeof(_wearerIdTxt[0]) - 1) {
          wearId += 1;
        } else if (i == 0 && wearId > 0) {
          wearId -= 1;
        } else if (i == 3 &&
                   (playCategoryNum <
                    sizeof(_playCategoryTxt) / sizeof(_playCategoryTxt[0]) -
                        1)) {
          playCategoryNum += 1;
        } else if (i == 2 && playCategoryNum > 0) {
          playCategoryNum -= 1;
        } else if (i == 4) {
          if (_isFixMode) {
            _isFixMode = false;
            _leds[0] = _colorVolumeMode;
            ;
            if (!_disableVolumeControl) {
              setAmpStepGain(_ampVolStep);
            }
          } else {
            _isFixMode = true;
            _leds[0] = _colorFixMode;
            setFixGain();
          }
          FastLED.show();
          audioManager::setIsFixMode(_isFixMode);
        }
        _isBtnPressed[i] = true;
        if (i != 4) {
          int tstep =
              (_isFixMode) ? _fixGainStep[playCategoryNum] : _ampVolStep;
          displayManager::updateOLED(&_display, playCategoryNum, wearId, tstep);
          audioManager::setPlayCategory(playCategoryNum);
          audioManager::setWearerId(wearId);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };

    // loop delay
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
#endif

#if defined(GENERAL_V2)
void TaskUI(void *args) {
  while (1) {
    // デバッグ用、電池残量表示
    // BQ27220_Cmd::printBatteryStats();
    // ボタン操作
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        uint8_t playCategoryNum = audioManager::getPlayCategory();
        uint8_t wearId = audioManager::getWearerId();
        audioManager::stopAudio();
        if (i == 2 &&
            wearId < sizeof(_wearerIdTxt) / sizeof(_wearerIdTxt[0]) - 1) {
          wearId += 1;
        } else if (i == 0 && wearId > 0) {
          wearId -= 1;
        } else if (i == 1) {
          if (playCategoryNum <
              sizeof(_playCategoryTxt) / sizeof(_playCategoryTxt[0]) - 1) {
            playCategoryNum += 1;
          } else {
            playCategoryNum = 0;
          }
        }
        _isBtnPressed[i] = true;
        displayManager::updateOLED(
            &_display, playCategoryNum, wearId,
            _fixGainStep[playCategoryNum]);
        audioManager::setPlayCategory(playCategoryNum);
        audioManager::setWearerId(wearId);
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };
    // loop delay
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
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

/////////////// execution ////////////////////////////////////
void setup() {
  // デバッグ用ボタン
  USBSerial.begin(115200);
  USBSerial.println("init Hapbeat wireless reciever");
  audioManager::initParamsEEPROM();
  vTaskDelay(5 / portTICK_PERIOD_MS);

#if defined(NECKLACE_V_1_3)
  // battery current sensing pins
  pinMode(BAT_CURRENT_PIN, INPUT);
  pinMode(DETECT_ANALOG_IN_PIN, INPUT);
#endif

#if defined(NECKLACE) || defined(NECKLACE_V_1_3)
  pinMode(AIN_VIBVOL_PIN, INPUT);
  // set device position as NECK = 0
  audioManager::setDevicePos(0);
#endif

#if defined(GENERAL) || defined(GENERAL_V2)
  audioManager::setDevicePos(5);
#endif

#if defined(NECKLACE) || defined(NECKLACE_V_1_3) || defined(GENERAL_V2)
  // ボタンピン設定
  for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
    pinMode(_SW_PIN[i], INPUT);
  };
  // Show LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(_leds, 1);
  // init I2S DAC
  pinMode(EN_I2S_DAC_PIN, OUTPUT);
  digitalWrite(EN_I2S_DAC_PIN, HIGH);
  // init _display
  pinMode(EN_OLED_PIN, OUTPUT);
  digitalWrite(EN_OLED_PIN, HIGH);
  displayManager::initOLED(&_display, DISP_ROT);
  int m_size = sizeof(_playCategoryTxt) / sizeof(_playCategoryTxt[0]);
  int w_size = sizeof(_wearerIdTxt) / sizeof(_wearerIdTxt[0]);
  int d_size = sizeof(_decibelTxt) / sizeof(_decibelTxt[0]);
  displayManager::setTitle(_playCategoryTxt, m_size, _wearerIdTxt, w_size,
                           _decibelTxt, d_size);
  // vibAmp
  pinMode(EN_VIBAMP_PIN, OUTPUT);
  // digitalWrite(EN_VIBAMP_PIN, LOW);
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  pinMode(AOUT_VIBVOL_PIN, OUTPUT);
  // D級アンプのゲイン決定
  pinMode(EN_MOTOR_PIN, OUTPUT);
  digitalWrite(EN_MOTOR_PIN, HIGH);
  // setFixGain内でupdateOLEDが呼ばれるので不要
  // displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
  //                            audioManager::getWearerId(), 0);
  setFixGain();

  _isFixMode = audioManager::getIsFixMode();
  if (_isFixMode) {
    _leds[0] = _colorFixMode;
  } else {
    _leds[0] = _colorVolumeMode;
  }
  FastLED.show();
#endif

#if defined(NECKLACE_V_1_3) || defined(GENERAL_V2)
  BQ27220_Cmd::setupBQ27220(SDA_PIN, SCL_PIN, BATTERY_CAPACITY);
#endif

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

  audioManager::readAllSoundFiles();
  audioManager::initAudioOut(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);

  // 原因は不明だが、TaskUI=>TaskAudioの順にすると、GENERALではボタンを押すまで動作しない。
  xTaskCreatePinnedToCore(TaskAudio, "TaskAudio", 4096, NULL, 0, &thp[1], 0);
  xTaskCreatePinnedToCore(TaskUI, "TaskUI", 4096, NULL, 2, &thp[0], 1);

#if defined(NECKLACE_V_1_3)
  xTaskCreatePinnedToCore(TaskCurrent, "TaskCurrent", 4096, NULL, 2, &thp[2],
                          1);
#endif
  // 4096
  // 無線通信の開始
#ifdef ESPNOW
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
#elif MQTT
  MQTT_manager::initMQTTclient(audioManager::PlaySndFromMQTTcallback,
                               statusCallback);
#endif
}
void loop() {
// ネックレスはこれで動いていたが、Generalは駄目。原因不明
#if defined(MQTT) && defined(NECKLACE_V_1_3)
  MQTT_manager::loopMQTTclient();
#endif
};