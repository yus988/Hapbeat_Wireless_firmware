#include <Arduino.h>

#include <iostream>
// ディスプレイ関連
#include <Wire.h>
// 自作ライブラリ
#include <BQ27220.h>
#include <FastLED.h>
#include <audioManager.h>
#include <espnow_manager.h>

#ifndef GENERAL
  #include <Adafruit_SSD1306.h>
  #include <displayManager.h>
#endif
/////////////////////// define pins /////////////////
#if defined(NECKLACE)
  // Audio pins
  #define I2S_BCLK_PIN 39
  #define I2S_DOUT_PIN 40
  #define I2S_LRCK_PIN 38
  #define I2S_MLCK_PIN 41
  #define EN_I2S_DAC 7
  // ピン番号はv1_3で変更予定
  #define EN_MOTOR_PIN 44
  // analog amp
  #define EN_VIBAMP_PIN 43
  #define AIN_VIBVOL_PIN 1
  #define AOUT_VIBBVOL_PIN 4
  // _display pins
  #define SCLK_PIN 17
  #define MOSI_PIN 18
  #define MISO_PIN 37
  #define OLED_DC_PIN 16
  #define OLED_RESET_PIN 15
  #define CS_PIN 14
  #define EN_OLED_PIN 47
  // Button pins
  #define SW1_VOL_P_PIN 13
  #define SW2_VOL_N_PIN 12
  #define SW3_SEL_P_PIN 2
  #define SW4_SEL_N_PIN 34
  #define SW5_ENTER_PIN 21
  // LED
  #define LED_PIN 3
  // _display params
  #define DISP_ROT 0
  // #define DISP_ROT 90 // 上下逆
  #define SCREEN_WIDTH 128  // OLED _display width, in pixels
  #define SCREEN_HEIGHT 32  // OLED _display height, in pixels

  #define FIX_GAIN_STEP 41
  #define VOLUME_THRESHOLD 60
#endif

#if defined(NECKLACE_V_1_3)
  // Audio pins
  #define I2S_BCLK_PIN 39
  #define I2S_DOUT_PIN 40
  #define I2S_LRCK_PIN 38
  #define I2S_MLCK_PIN 41
  #define EN_I2S_DAC 11
  // ピン番号はv1_3で変更予定
  #define EN_MOTOR_PIN 9
  // analog amp
  #define EN_VIBAMP_PIN 10
  #define AIN_VIBVOL_PIN 1
  #define AOUT_VIBBVOL_PIN 6
  // _display pins
  #define SCLK_PIN 17
  #define MOSI_PIN 18
  #define MISO_PIN 37
  #define OLED_DC_PIN 16
  #define OLED_RESET_PIN 15
  #define CS_PIN 14
  #define EN_OLED_PIN 21
  // Button pins
  #define SW1_VOL_P_PIN 26
  #define SW2_VOL_N_PIN 47
  // 基板上はこちらが正しいが、旧版に合わせるため逆にする
  // #define SW3_SEL_P_PIN 33
  // #define SW4_SEL_N_PIN 34
  #define SW3_SEL_P_PIN 34
  #define SW4_SEL_N_PIN 33
  //
  #define SW5_ENTER_PIN 48
  // #define SW5_ENTER_PIN 0

  // LED
  #define LED_PIN 8
// _display params

  #define COLOR_FIX_MODE CRGB(10, 10, 10)
  #define COLOR_VOLUME_MODE CRGB(0, 0, 10)
  #define COLOR_DANGER_MODE CRGB(10, 0, 0)

  #define DISP_ROT 0
  // #define DISP_ROT 90 // 上下逆
  #define SCREEN_WIDTH 128  // OLED _display width, in pixels
  #define SCREEN_HEIGHT 32  // OLED _display height, in pixels

  #define FIX_GAIN_STEP 41
  #define VOLUME_THRESHOLD 100

  // update from v1_2
  #define BQ27x_PIN 2
  #define SDA_PIN 3
  #define SCL_PIN 4
  #define BAT_CURRENT_PIN 5
  #define R5_BATVOL_PIN 7
  #define DETECT_ANALOG_IN_PIN 45

// 定数の定義
  #define SHUNT_RESISTANCE 0.01  // シャント抵抗 (オーム)
  #define INA_GAIN 50.0          // INA180A2IDBVRのゲイン (V/V)
  #define ADC_MAX 4095           // ADCの分解能
  #define V_REF 3.3              // アナログ基準電圧 (V)
  #define BATTERY_CAPACITY 3000

#endif

#if defined(NECKLACE) || defined(NECKLACE_V_1_3)
//////////// Variables //////////////////////////////////////
// LED
CRGB _leds[1];
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                          OLED_DC_PIN, OLED_RESET_PIN, CS_PIN);
// 表示させる文字を入力
// const char *_menuTxt[] = {"テニス", "野球", "バスケ"};
const char *_menuTxt[] = {"ゲーム", "動画", "hoge"};
const char *_wearerIdTxt[] = {"ALL", "競技", "会場", "4", "5", "6"};
// see pam8003 datasheet p.7
const char *_decibelTxt[] = {
    "-75",  "-40",  "-34",  "-28",  "-22",  "-16",  "-10",  "-7.5",
    "-5",   "-2.5", "0",    "1.5",  "3.0",  "4.0",  "4.4",  "4.8",
    "5.2",  "5.6",  "6.0",  "6.4",  "6.8",  "7.2",  "7.6",  "8.0",
    "8.4",  "8.8",  "9.2",  "9.6",  "10.0", "10.4", "10.8", "11.2",
    "11.6", "12.0", "12.4", "12.8", "13.2", "13.6", "14.0", "14.4",
    "14.8", "15.2", "15.6", "16.0", "16.4", "16.8", "17.2", "17.6",
    "18.0", "18.4", "18.8", "19.2", "19.6", "20.0", "20.4", "20.8",
    "21.2", "21.6", "22.0", "22.4", "22.8", "23.2", "23.6", "24.0"};
int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN, SW3_SEL_P_PIN, SW4_SEL_N_PIN,
                 SW5_ENTER_PIN};
bool _isBtnPressed[] = {false, false, false, false, false};

// volume related variables
int _prevAIN = 0;
int _currAIN = 0;
bool _isFixMode;
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
#endif

////////////////////////////////// define tasks ////////////////////////////////
TaskHandle_t thp[3];

#if defined(NECKLACE) || defined(NECKLACE_V_1_3)
void setFixGain(bool updateOLED = true) {
  // 15dBにあたるステップ数0--63をanalogWrite0--255に変換する
  analogWrite(AOUT_VIBBVOL_PIN, map(FIX_GAIN_STEP, 0, 63, 0, 255));
  USBSerial.println(map(FIX_GAIN_STEP, 0, 63, 0, 255));
  if (updateOLED) {
    displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                               audioManager::getWearerId(), FIX_GAIN_STEP);
  }
}

// PAMの電圧を下げる
void setAmpStepGain(int step, bool updateOLED = true) {
  // USBSerial.println(_currAIN);
  int volume = map(_currAIN, 0, 4095, 0, 255);
  analogWrite(AOUT_VIBBVOL_PIN, volume);
  // ディスプレイにdB表示用のステップ数変換
  if (updateOLED) {
    displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                               audioManager::getWearerId(), step);
  }
}

// 優先度は最低にする
void TaskAudio(void *args) {
  while (1) {
    audioManager::playAudioInLoop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void TaskCurrent(void *args) {
  int adc_value;
  float current;
  const float current_thresholds[2] = {5.0, 5.5};  // 電流値の閾値 (A)
  // シャットダウンサイクル数。大きいほど、長時間閾値越えを許容する。閾値に応じて変える
  const int shutdownCycles[2] = {20, 4};
  int shutdownCounter[2] = {0, 0};
  const int restoreCycles = 100;  // 復帰サイクル数
  int restoreCounter = 0;         // カウンタの初期化
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
          _leds[0] = COLOR_VOLUME_MODE;
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
        abs(_currAIN - _prevAIN) > VOLUME_THRESHOLD) {
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
        } else if (i == 3 && (playCategoryNum <
                              sizeof(_menuTxt) / sizeof(_menuTxt[0]) - 1)) {
          playCategoryNum += 1;
        } else if (i == 2 && playCategoryNum > 0) {
          playCategoryNum -= 1;
        } else if (i == 4) {
          if (_isFixMode) {
            _isFixMode = false;
            _leds[0] = COLOR_VOLUME_MODE;
            ;
            if (!_disableVolumeControl) {
              setAmpStepGain(_ampVolStep);
            }
          } else {
            _isFixMode = true;
            _leds[0] = COLOR_FIX_MODE;
            ;
            setFixGain();
          }
          FastLED.show();
          audioManager::setIsFixMode(_isFixMode);
        }
        _isBtnPressed[i] = true;
        if (i != 4) {
          int tstep = (_isFixMode) ? FIX_GAIN_STEP : _ampVolStep;
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
        } else if (i == 1) {
          devicePos += 1;
          // audioManager::setDevicePos(devicePos);
        } else if (i == 2) {
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
        vTaskDelay(50 / portTICK_PERIOD_MS);
      }
      if (digitalRead(_SW_PIN[i]) && _isBtnPressed[i]) {
        _isBtnPressed[i] = false;
        vTaskDelay(50 / portTICK_PERIOD_MS);
      }
    }
  }
}

#endif

/////////////// execution ////////////////////////////////////
void setup() {
  // デバッグ用ボタン
  USBSerial.begin(115200);
  USBSerial.println("init Hapbeat wireless reciever");
  audioManager::initParamsEEPROM();
  vTaskDelay(5 / portTICK_PERIOD_MS);

#if defined(NECKLACE) || defined(NECKLACE_V_1_3)
  // ボタンピン設定
  for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
    pinMode(_SW_PIN[i], INPUT);
  };
  // Show LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(_leds, 1);
  // data read pins
  pinMode(BAT_CURRENT_PIN, INPUT);
  pinMode(DETECT_ANALOG_IN_PIN, INPUT);
  // init I2S DAC
  pinMode(EN_I2S_DAC, OUTPUT);
  // digitalWrite(EN_I2S_DAC, LOW);
  digitalWrite(EN_I2S_DAC, HIGH);
  // init _display
  pinMode(EN_OLED_PIN, OUTPUT);
  digitalWrite(EN_OLED_PIN, HIGH);
  displayManager::initOLED(&_display, DISP_ROT);
  int m_size = sizeof(_menuTxt) / sizeof(_menuTxt[0]);
  int w_size = sizeof(_wearerIdTxt) / sizeof(_wearerIdTxt[0]);
  int d_size = sizeof(_decibelTxt) / sizeof(_decibelTxt[0]);
  displayManager::setTitle(_menuTxt, m_size, _wearerIdTxt, w_size, _decibelTxt,
                           d_size);
  // vibAmp
  pinMode(EN_VIBAMP_PIN, OUTPUT);
  // digitalWrite(EN_VIBAMP_PIN, LOW);
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  pinMode(AIN_VIBVOL_PIN, INPUT);
  pinMode(AOUT_VIBBVOL_PIN, OUTPUT);
  analogWrite(AOUT_VIBBVOL_PIN, 100);
  // analogWrite(AOUT_VIBBVOL_PIN, 200); // set vibration volume
  pinMode(EN_MOTOR_PIN, OUTPUT);
  digitalWrite(EN_MOTOR_PIN, HIGH);
  displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                             audioManager::getWearerId(), 0);
  // set device position as NECK = 0
  audioManager::setDevicePos(0);
  // audioManager::setDevicePos(5);
  _isFixMode = audioManager::getIsFixMode();
  if (_isFixMode) {
    _leds[0] = COLOR_FIX_MODE;
  } else {
    _leds[0] = COLOR_VOLUME_MODE;
  }
  FastLED.show();
#endif

#ifdef NECKLACE_V_1_3
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
  // flash memory 内のファイルを読み込み
  audioManager::readAllSoundFiles();
  audioManager::initAudioOut(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);
  // espnowの開始
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
  // #ifdef GENERAL
  //   //
  //   MAXをenableする前に、initAudioOutを実行してesp32からI2S信号を出す必要あり
  //   // 出てないとMOSFETのゲート電圧がおかしくなり、焼ける
  //   // ここで切るべきなのは、MOSFETへの電力供給
  // #endif

  // 原因は不明だが、TaskUI=>TaskAudioの順にすると、GENERALではボタンを押すまで動作しない。
  xTaskCreatePinnedToCore(TaskAudio, "TaskAudio", 4096, NULL, 0, &thp[1], 0);
  xTaskCreatePinnedToCore(TaskUI, "TaskUI", 4096, NULL, 2, &thp[0], 1);
  xTaskCreatePinnedToCore(TaskCurrent, "TaskCurrent", 4096, NULL, 2, &thp[2],
                          1);
  // 4096
}
void loop() {};