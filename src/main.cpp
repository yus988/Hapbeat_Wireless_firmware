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

//////////////// variables  /////////////////////////////
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

TaskHandle_t thp[3];  // 環境によって必要タスク数が変わるので注意
bool _isFixMode;

// LED
CRGB _leds[1];
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                          OLED_DC_PIN, OLED_RESET_PIN, CS_PIN);

#if defined(NECKLACE_V_1_3)
// see pam8003 datasheet p.7
int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN, SW3_SEL_P_PIN, SW4_SEL_N_PIN,
                 SW5_ENTER_PIN};
bool _isBtnPressed[] = {false, false, false, false, false};

// volume related variables
int _prevAIN = 0;
int _currAIN = 0;
bool _disableVolumeControl = false;
uint8_t _ampVolStep;

// 電流を計算するための関数
float calculateCurrent(int adc_value) {
  // ADCの値を電圧に変換
  float voltage = (adc_value * V_REF) / ADC_MAX;
  // 電圧を電流に変換
  float current = voltage / (INA_GAIN * SHUNT_RESISTANCE);
  return current;
}
#endif

#if defined(GENERAL_V2)
// see pam8003 datasheet p.7

int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN};
bool _isBtnPressed[] = {false, false};
#endif

//////////////// general functions  /////////////////////////////

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
      map(FIX_GAIN_STEP[audioManager::getPlayCategory()], 0, 63, 0, 255));
  if (updateOLED) {
    displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                               audioManager::getWearerId(),
                               FIX_GAIN_STEP[audioManager::getPlayCategory()]);
  }
}

////////////////////////////////// define tasks ////////////////////////////////
void TaskAudio(void *args) {
  while (1) {
    audioManager::playAudioInLoop();
    delay(50);
  }
}

  // xTaskCreatePinnedToCore だと何故か安定しない
void TaskMQTT(void *args) {
  while (1) {
    MQTT_manager::loopMQTTclient();
    // 15秒未満なら遅延許容度の兼ね合い。パラメータにしても良いかも。
    delay(200);
  }
}

#if defined(NECKLACE_V_1_3)
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
        _leds[0] = COLOR_DANGER_MODE;
        FastLED.show();
        restoreCounter = 0;  // カウンタリセット
      }
    } else {
      if (restoreCounter >= restoreCycles) {
        _disableVolumeControl = false;  // 音量操作を有効化
        digitalWrite(EN_VIBAMP_PIN, HIGH);
        if (_isFixMode) {
          setFixGain(false);
          _leds[0] = COLOR_FIX_MODE;
        } else {
          setAmpStepGain(_ampVolStep, false);
          _leds[0] = COLOR_VOL_MODE;
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
            wearId < sizeof(WEARER_ID_TXT) / sizeof(WEARER_ID_TXT[0]) - 1) {
          wearId += 1;
        } else if (i == 0 && wearId > 0) {
          wearId -= 1;
        } else if (i == 3 &&
                   (playCategoryNum <
                    sizeof(PLAY_CATEGORY_TXT) / sizeof(PLAY_CATEGORY_TXT[0]) -
                        1)) {
          playCategoryNum += 1;
        } else if (i == 2 && playCategoryNum > 0) {
          playCategoryNum -= 1;
        } else if (i == 4) {
          if (_isFixMode) {
            _isFixMode = false;
            _leds[0] = COLOR_VOL_MODE;
            ;
            if (!_disableVolumeControl) {
              setAmpStepGain(_ampVolStep);
            }
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
          int tstep =
              (_isFixMode) ? FIX_GAIN_STEP[playCategoryNum] : _ampVolStep;
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
  // ゆくゆくはアプリケーションに応じてTaskUIをファイル別に分ける。
  #ifdef COLOR_SENSOR
void TaskUI(void *args) {
  while (1) {
    // デバッグ用、電池残量表示
    // BQ27220_Cmd::printBatteryStats();
    // ボタン操作
    for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
      if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
        // uint8_t playCategoryNum = audioManager::getPlayCategory();
        // uint8_t wearId = audioManager::getWearerId();
        if (i == 1) {
          bool isLimitEnable = audioManager::getIsLimitEnable();
          USBSerial.println("Button 1");
          USBSerial.print("Limit Enabled Status before toggle: ");
          USBSerial.println(isLimitEnable ? "true" : "false");

          if (isLimitEnable) {
            audioManager::setIsLimitEnable(false);
          } else {
            audioManager::setIsLimitEnable(true);
          }

          USBSerial.print("Limit Enabled Status after toggle: ");
          USBSerial.println(audioManager::getIsLimitEnable() ? "true"
                                                             : "false");

          int msgIdx = (isLimitEnable) ? 0 : 1;
          if (LIMIT_ENABLE_MSG[msgIdx] != nullptr) {
            statusCallback(LIMIT_ENABLE_MSG[msgIdx]);
          } else {
            USBSerial.println("Error: Message pointer is null");
          }
          // audioManager::playAudio(0, 30);
        } else if (i == 0) {
          USBSerial.println("Button 0");
          audioManager::stopAudio();
        }
        _isBtnPressed[i] = true;
        // displayManager::updateOLED(&_display, playCategoryNum, wearId,
        //                            FIX_GAIN_STEP[playCategoryNum]);
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) _isBtnPressed[i] = false;
    };
    // loop delay
    delay(100);
  }
}
  #else
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
            wearId < sizeof(WEARER_ID_TXT) / sizeof(WEARER_ID_TXT[0]) - 1) {
          wearId += 1;
        } else if (i == 0 && wearId > 0) {
          wearId -= 1;
        } else if (i == 1) {
          if (playCategoryNum <
              sizeof(PLAY_CATEGORY_TXT) / sizeof(PLAY_CATEGORY_TXT[0]) - 1) {
            playCategoryNum += 1;
          } else {
            playCategoryNum = 0;
          }
        }
        _isBtnPressed[i] = true;
        displayManager::updateOLED(&_display, playCategoryNum, wearId,
                                   FIX_GAIN_STEP[playCategoryNum]);
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

#endif

/////////////// execution ////////////////////////////////////
void setup() {
  // デバッグ用ボタン
  USBSerial.begin(115200);
  USBSerial.println("init Hapbeat wireless reciever");
  audioManager::initParamsEEPROM();
  delay(5);
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
  int m_size = sizeof(PLAY_CATEGORY_TXT) / sizeof(PLAY_CATEGORY_TXT[0]);
  int w_size = sizeof(WEARER_ID_TXT) / sizeof(WEARER_ID_TXT[0]);
  int d_size = sizeof(DECIBEL_TXT) / sizeof(DECIBEL_TXT[0]);
  displayManager::setTitle(PLAY_CATEGORY_TXT, m_size, WEARER_ID_TXT, w_size,
                           DECIBEL_TXT, d_size);
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
    _leds[0] = COLOR_FIX_MODE;
  } else {
    _leds[0] = COLOR_VOL_MODE;
  }
  FastLED.show();

#if defined(NECKLACE_V_1_3)
  // battery current sensing pins
  pinMode(BAT_CURRENT_PIN, INPUT);
  pinMode(DETECT_ANALOG_IN_PIN, INPUT);
  pinMode(AIN_VIBVOL_PIN, INPUT);
  // set device position as NECK = 0
  audioManager::setDevicePos(0);
  xTaskCreatePinnedToCore(TaskCurrent, "TaskCurrent", 4096, NULL, 2, &thp[2],
                          1);
#endif

#if defined(GENERAL_V2)
  audioManager::setDevicePos(5);
#endif

#ifdef ESPNOW
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
#elif MQTT
  audioManager::setLimitIds(LIMITED_IDS,
                            sizeof(LIMITED_IDS) / sizeof(LIMITED_IDS[0]));
  MQTT_manager::initMQTTclient(audioManager::PlaySndFromMQTTcallback,
                               statusCallback);
#endif
  xTaskCreatePinnedToCore(TaskAudio, "TaskAudio", 2048, NULL, 20, &thp[1], 1);
  xTaskCreatePinnedToCore(TaskUI, "TaskUI", 2048, NULL, 23, &thp[0], 1);
  BQ27220_Cmd::setupBQ27220(SDA_PIN, SCL_PIN, BATTERY_CAPACITY);
  audioManager::readAllSoundFiles();
  audioManager::initAudioOut(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);
}
void loop() {
#ifdef MQTT
  // xTaskCreatePinnedToCore だと何故か安定しない
  MQTT_manager::loopMQTTclient();
  delay(200);
#endif
};

// 優先度は 低 0-24 高
// BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode,
//                                    const char *constpcName,
//                                    const uint32_t usStackDepth,
//                                    void *constpvParameters,
//                                    UBaseType_t uxPriority,
//                                    TaskHandle_t *constpvCreatedTask,
//                                    const BaseType_t xCoreID)
