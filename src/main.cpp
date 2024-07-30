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

#include "adjustParams.h"
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
unsigned long _lastDisplayUpdate =
    0;  // ディスプレイが最後に更新された時刻を保持
unsigned long _lastBatStatusUpdate =
    0;  // ディスプレイが最後に更新された時刻を保持
// LED
CRGB _leds[1];
CRGB _currentColor;
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

void vibrationNotify() {
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  audioManager::setDataID(2, ID_MSG.notify);
  audioManager::stopAudio(2);
  audioManager::playAudio(2, 30);
}

// 現状はstatus=表示する文字列となっているが、
// 細かく設定したいなら、statusに応じて文とスタイルを別途定義すればよい。
// コールバック関数の引数を変えると変更箇所が多くなるので非推奨。
void showStatusText(const char *status) {
  _display.ssd1306_command(SSD1306_DISPLAYON);
  _display.clearDisplay();
  _display.setCursor(0, 0);
  if (status == "connection failed") {
    vibrationNotify();
    displayManager::printEfont(&_display, "WiFiへの接続を\n確認してください", 0,
                               0);
  } else {
    displayManager::printEfont(&_display, status, 0, 0);
  }
  _display.display();
  _lastDisplayUpdate = millis();  // 画面更新時刻をリセット
}

// 描画場所など指定したい場合はこれを使う
void showTextWithParams(const char *text, uint8_t posX, uint8_t posY,
                        bool isClearDisplay) {
  _display.ssd1306_command(SSD1306_DISPLAYON);
  if (isClearDisplay) {
    _display.clearDisplay();
  }
  _display.setCursor(posX, posY);
  displayManager::printEfont(&_display, text, posX, posY);
  _display.display();
  _lastDisplayUpdate = millis();  // 画面更新時刻をリセット
}

void manageBatteryStatus(bool showDisplay = false) {
  int voltage = lipo.voltage();
  int soc = (voltage - 3500) * 100 / (4100 - 3500);
  if (soc < 0) {
    soc = 0;
  } else if (soc > 100) {
    soc = 100;
  }
  uint8_t posX = 0;
  uint8_t posY = 8;
  _lastBatStatusUpdate = millis();
  std::string text;
  // 電池残量が所定の値以下になったら振動再生
  if (voltage < BAT_NOTIFY_VOL || soc < BAT_NOTIFY_SOC) {
    vibrationNotify();
    text = "充電してください";
  } else if (showDisplay) {
    std::string socStr = std::to_string(soc);  // 数値を文字列に変換
    std::string voltageStr = std::to_string(voltage);  // 数値を文字列に変換
    text = std::string(socStr) + "%" + " : " + std::string(voltageStr) +
           "mV";  // C++のstd::stringを使用して文字列を結合
  } else {
    return;
  }
  _display.ssd1306_command(SSD1306_DISPLAYON);  // ディスプレイを点灯させる
  _display.clearDisplay();
  _display.setCursor(posX, posY);  // カーソルを設定

  displayManager::printEfont(&_display, text.c_str(), posX,
                             posY);  // 文字列と座標を指定して表示
  _display.display();                // ディスプレイに表示
  _lastDisplayUpdate = millis();     // 画面更新時刻をリセット
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

// 待機中に極力電源オフ
void enableSleepMode() {
  // DISPLAY_TIMEOUT 秒が経過した場合、ディスプレイを消灯
  _display.ssd1306_command(SSD1306_DISPLAYOFF);
  // fill_solid(_leds, 1, CRGB::Black);  // すべてのLEDを黒色に設定。
  // FastLED.show();                     // LEDの色の変更を適用。
  digitalWrite(EN_VIBAMP_PIN, LOW);
}

//////////////////////// コールバック関数の定義 ////////////////////////
void messageReceived(char *topic, byte *payload, unsigned int length) {
  USBSerial.print("Message arrived in topic: ");
  USBSerial.println(topic);
  USBSerial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    USBSerial.print((char)payload[i]);
  }
  USBSerial.println();
}

void MQTTcallback(char *topic, byte *payload, unsigned int length) {
  // 各種ICをON
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  audioManager::PlaySndFromMQTTcallback(topic, payload, length);
}

////////////////////////////////// define tasks ////////////////////////////////
#ifdef ESPNOW
void TaskAudio(void *args) {
  while (1) {
    audioManager::playAudioInLoop();
    delay(50);
  }
}
#elif MQTT
void TaskAudio(void *args) {
  // MQTT_manager::getIsWiFiConnected()
  while (1) {
    audioManager::playAudioInLoop();
    delay(50);
  }
}
#endif

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
// color_sensor 用タスク
void TaskUI(void *args) {
  while (1) {
    // 所定の時間後に消灯。ただし音声再生中は実行しない
    if (millis() - _lastDisplayUpdate > DISPLAY_TIMEOUT &&
        audioManager::getIsPlaying() == false) {
      enableSleepMode();
    }
    if (millis() - _lastBatStatusUpdate > BATTERY_STATUS_INTERVAL) {
      manageBatteryStatus(false);
    }

    if (MQTT_manager::getIsWiFiConnected()) {
      // デバッグ用、電池残量表示
      // BQ27220_Cmd::printBatteryStats();
      // manageBatteryStatus();
      // ボタン操作
      for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
        if (!digitalRead(_SW_PIN[i]) && !_isBtnPressed[i]) {
          // ボタン押下で表示
          _display.ssd1306_command(SSD1306_DISPLAYON);
          _lastDisplayUpdate = millis();  // 画面更新時刻をリセット
          if (i == 1) {
            // モード切替
            bool isLimitEnable = audioManager::getIsLimitEnable();
            if (isLimitEnable) {
              audioManager::setIsLimitEnable(false);
              _currentColor = COLOR_FIX_MODE;
            } else {
              audioManager::setIsLimitEnable(true);
              _currentColor = COLOR_VOL_MODE;
            }
            int msgIdx = (isLimitEnable) ? 0 : 1;
            if (LIMIT_ENABLE_MSG[msgIdx] != nullptr) {
              showTextWithParams(LIMIT_ENABLE_MSG[msgIdx], 0, 8, true);
            } else {
              USBSerial.println("Error: Message pointer is null");
            }
            // vibrationNotify();
            // audioManager::playAudio(0, 30);
          } else if (i == 0) {
            USBSerial.println("Button 0");
            if (audioManager::getIsPlaying() == false) {
              manageBatteryStatus(true);
            }
            audioManager::stopAudio();
          }
          _leds[0] = _currentColor;
          FastLED.show();
          _isBtnPressed[i] = true;
        }
        if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) {
          _isBtnPressed[i] = false;
        }
      };
      // loop delay
    }
    delay(190);
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
  // vibAmp
  pinMode(EN_VIBAMP_PIN, OUTPUT);
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  pinMode(AOUT_VIBVOL_PIN, OUTPUT);
  // D級アンプの初期設定＆ゲイン決定
  pinMode(EN_MOTOR_PIN, OUTPUT);
  digitalWrite(EN_MOTOR_PIN, HIGH);
  pinMode(BQ27x_PIN, INPUT);
  _isFixMode = audioManager::getIsFixMode();
  if (_isFixMode) {
    _currentColor = COLOR_FIX_MODE;
  } else {
    _currentColor = COLOR_VOL_MODE;
  }
  _leds[0] = _currentColor;
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
  setFixGain();  // 実行しないと VibAmpVolume = 0 のままなので必須

  // ここはタスク依存
  int m_size = sizeof(PLAY_CATEGORY_TXT) / sizeof(PLAY_CATEGORY_TXT[0]);
  int w_size = sizeof(WEARER_ID_TXT) / sizeof(WEARER_ID_TXT[0]);
  int d_size = sizeof(DECIBEL_TXT) / sizeof(DECIBEL_TXT[0]);
  displayManager::setTitle(PLAY_CATEGORY_TXT, m_size, WEARER_ID_TXT, w_size,
                           DECIBEL_TXT, d_size);
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
#elif MQTT
  setFixGain(false);  // 実行しないと VibAmpVolume = 0 のままなので必須
  audioManager::setLimitIds(LIMITED_IDS,
                            sizeof(LIMITED_IDS) / sizeof(LIMITED_IDS[0]));
  audioManager::setStatusCallback(showStatusText);
  // DISP_MSG 配列の内容を messages ベクターに追加
  for (const auto &msg : DISP_MSG) {
    audioManager::setMessageData(msg.message, msg.id);
  };
  MQTT_manager::initMQTTclient(MQTTcallback, showStatusText);
#endif
  while (!MQTT_manager::getIsWiFiConnected()) {
    USBSerial.println("waiting for WiFi connection...");
    delay(500);  // 少し待って再試行
  };
  audioManager::readAllSoundFiles();
  audioManager::initAudioOut(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);
  BQ27220_Cmd::setupBQ27220(SDA_PIN, SCL_PIN, BATTERY_CAPACITY);
  xTaskCreatePinnedToCore(TaskAudio, "TaskAudio", 2048, NULL, 20, &thp[1], 1);
  xTaskCreatePinnedToCore(TaskUI, "TaskUI", 2048, NULL, 23, &thp[0], 1);
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
