#include <Arduino.h>
#include <iostream>

// ディスプレイ関連
#include <Wire.h>
// 自作ライブラリ
#include <espnow_manager.h>
#include <audioManager.h>
#include <FastLED.h>

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
  // GSELは無いのでダミー。後々setGainを修正
  #define G_SEL_A_PIN 9
  #define G_SEL_B_PIN 10
  // ピン番号はv1_3で変更予定
  #define EN_MOTOR_PIN 44
  // analog amp
  #define EN_VIBAMP_PIN 43
  #define AIN_VIBVOL_PIN 1
  #define AOUT_VIBBVOL_PIN 4
  // Display pins
  #define SCLK_PIN 17
  #define MOSI_PIN 18
  #define MISO_PIN 37
  #define OLED_DC_PIN 16
  #define OLED_RESET_PIN 15
  #define CS_PIN 14
  #define EN_OLED_PIN 47
  // Button pins
  #define SW0_VOL_P_PIN 13
  #define SW1_VOL_N_PIN 12
  #define SW2_SEL_P_PIN 2
  #define SW3_SEL_N_PIN 34
  #define SW4_ENTER_PIN 21
  // LED
  #define LED_PIN 3
  // params
  #define SCREEN_WIDTH 128  // OLED display width, in pixels
  #define SCREEN_HEIGHT 32  // OLED display height, in pixels

//////////// Variables //////////////////////////////////////
// LED
CRGB leds[1];
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                         OLED_DC_PIN, OLED_RESET_PIN, CS_PIN);
// 表示させる文字を入力
// const char *menuTxt[] = {"テニス", "野球", "バスケ"};
const char *menuTxt[] = {"FPSゲーム", "音声認識", "hoge"};
const char *wearerIdTxt[] = {"0", "1", "2", "3", "4", "5", "6"};
int SW_PIN[] = {SW0_VOL_P_PIN, SW1_VOL_N_PIN, SW2_SEL_P_PIN, SW3_SEL_N_PIN,
                SW4_ENTER_PIN};
bool isBtnPressed[] = {false, false, false, false, false};

int prevAIN = 0;
int currAIN = 0;

uint8_t wearerId = 0;

#endif

#ifdef NECKLACE_V1
  // Audio pins
  #define I2S_BCLK_PIN 39
  #define I2S_DOUT_PIN 40
  #define I2S_LRCK_PIN 38
  #define SDMODE_PIN 21  // for MAX98357A
  #define G_SEL_A_PIN 41
  #define G_SEL_B_PIN 42
  #define G_SEL_C_PIN 45
  // Display pins
  #define SCLK_PIN 35
  #define MOSI_PIN 36
  #define MISO_PIN 37
  #define OLED_DC_PIN 34
  #define OLED_RESET_PIN 33  // Reset pin # (or -1 if sharing Arduino reset pin)
  #define CS_PIN 47
  #define EN_OLED_PIN 14
  // Button pins
  #define SW0_VOL_P_PIN 13
  #define SW1_VOL_N_PIN 12
  #define SW2_SEL_P_PIN 11
  #define SW3_SEL_N_PIN 10
  #define SW4_ENTER_PIN 9
  // LED
  #define LED_PIN 2
  #define MAX_GAIN_NUM 3
  // params
  #define SCREEN_WIDTH 128  // OLED display width, in pixels
  #define SCREEN_HEIGHT 32  // OLED display height, in pixels

//////////// Variables //////////////////////////////////////
// LED
CRGB leds[1];
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                         OLED_DC_PIN, OLED_RESET_PIN, CS_PIN);
// 表示させる文字を入力
const char *menuTxt[3] = {"テニス", "野球", "バスケ"};
// const char *menuTxt[3] = {"FPSゲーム", "音声認識", "hoge"};
const char *gainTxt[4] = {"1", "2", "3", "4"};

int SW_PIN[5] = {SW0_VOL_P_PIN, SW1_VOL_N_PIN, SW2_SEL_P_PIN, SW3_SEL_N_PIN,
                 SW4_ENTER_PIN};
bool isBtnPressed[5] = {false, false, false, false, false};
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

CRGB leds[4];
int SW_PIN[] = {SW1_PIN, SW2_PIN, SW3_PIN};
bool isBtnPressed[] = {false, false, false};  // control state of being pressed
uint8_t gainNum;
// 電源OFFしても覚えておきたい
uint8_t devicePos = 0;
uint8_t wearerId = 0;

#endif

TaskHandle_t thp[2];

void TaskAudio(void *args) {
  audioManager::playAudioInLoop();
  delay(1);
  // ets_delay_us(1);
}

#ifdef NECKLACE
// ボタン操作。将来的に別のコアで動作させたい
void TaskUI(void *args) {
  for (int i = 0; i < sizeof(SW_PIN) / sizeof(SW_PIN[0]); i++) {
    if (!digitalRead(SW_PIN[i]) && !isBtnPressed[i]) {
      uint8_t playCategoryNum = audioManager::getPlayCategory();
      audioManager::stopAll();
      // 各ボタン毎の操作 0,1 = 装着者ID, 2,3 = channel
      if (i == 0 &&
          wearerId < sizeof(wearerIdTxt) / sizeof(wearerIdTxt[0]) - 1) {
        wearerId += 1;
      } else if (i == 1 && wearerId > 0) {
        wearerId -= 1;
      } else if (i == 2 &&
                 (playCategoryNum < sizeof(menuTxt) / sizeof(menuTxt[0]) - 1)) {
        playCategoryNum += 1;
      } else if (i == 3 && playCategoryNum > 0) {
        playCategoryNum -= 1;
      }
      displayManager::updateOLED(&display, playCategoryNum, wearerId);
      isBtnPressed[i] = true;
      audioManager::setPlayCategory(playCategoryNum);
      audioManager::setWearerId(wearerId);
      delay(1);
    }
    if (digitalRead(SW_PIN[i]) && isBtnPressed[i]) isBtnPressed[i] = false;
  };
  // control pam8003 volume
  currAIN = analogRead(AIN_VIBVOL_PIN);
  if (currAIN != prevAIN) {
    // USBSerial.println(currAIN);
    int volume = map(currAIN, 0, 4095, 0, 255);
    analogWrite(AOUT_VIBBVOL_PIN, volume);
  }
  prevAIN = currAIN;
  delay(50);
}
#endif

// v1_2に完全移行したら消す
#ifdef NECKLACE_V1
void TaskUI(void *args) {
  for (int i = 0; i < sizeof(SW_PIN) / sizeof(SW_PIN[0]); i++) {
    if (!digitalRead(SW_PIN[i]) && !isBtnPressed[i]) {
      uint8_t playCategoryNum = audioManager::getPlayCategory();
      uint8_t gainNum = audioManager::getGain();
      audioManager::stopAll();
      // 各ボタン毎の操作 0,1 = Vol, 2,3 = channel
      if (i == 0 && gainNum < MAX_GAIN_NUM) {
        gainNum += 1;
      } else if (i == 1 && gainNum > 0) {
        gainNum -= 1;
      } else if (i == 2 &&
                 (playCategoryNum < sizeof(menuTxt) / sizeof(menuTxt[0]) - 1)) {
        playCategoryNum += 1;
      } else if (i == 3 && playCategoryNum > 0) {
        playCategoryNum -= 1;
      }
      displayManager::updateOLED(&display, playCategoryNum, gainNum);
      isBtnPressed[i] = true;
      audioManager::setGain(G_SEL_A_PIN, G_SEL_B_PIN, gainNum);
      audioManager::setPlayCategory(playCategoryNum);
      delay(1);
    }
    if (digitalRead(SW_PIN[i]) && isBtnPressed[i]) isBtnPressed[i] = false;
  };
}
#endif

#ifdef GENERAL
void TaskUI(void *args) {
  for (int i = 0; i < sizeof(SW_PIN) / sizeof(SW_PIN[0]); i++) {
    if (!digitalRead(SW_PIN[i]) && !isBtnPressed[i]) {
      uint8_t playCategoryNum = audioManager::getPlayCategory();
      uint8_t gainNum = audioManager::getGain();
      audioManager::stopAll();
      // 各ボタン毎の操作 0=wearer, 1=pos, 2=gain
      if (i == 0) {
        if (wearerId < 3) {
          wearerId += 1;
        } else {
          wearerId = 0;
        }
        for (int j = 0; j < 4; j++) {
          if (j == wearerId) {
            leds[j].setRGB(5, 5, 5);
          } else {
            leds[j].setRGB(0, 0, 0);
          }
        }
        FastLED.show();
        USBSerial.println(wearerId);
      } else if (i == 1) {
        devicePos += 1;
        // audioManager::setDevicePos(devicePos);
      } else if (i == 2) {
        gainNum += 1;
        if (gainNum > MAX_GAIN_NUM) {
          gainNum = 0;
        }
        audioManager::setGain(G_SEL_A_PIN, G_SEL_B_PIN, gainNum);
      }
      isBtnPressed[i] = true;
      delay(50);
    }
    if (digitalRead(SW_PIN[i]) && isBtnPressed[i]) {
      isBtnPressed[i] = false;
      delay(50);
    }
  }
}
#endif

/////////////// execution ////////////////////////////////////
void setup() {
  // デバッグ用ボタン
  USBSerial.begin(115200);
  USBSerial.println("init Hapbeat wireless reciever");
#ifdef NECKLACE
  // ボタンピン設定
  for (int i = 0; i < sizeof(SW_PIN) / sizeof(SW_PIN[0]); i++) {
    pinMode(SW_PIN[i], INPUT);
  };
  // Show LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, 1);
  leds[0].setRGB(10, 10, 10);
  FastLED.show();

  // init I2S DAC
  pinMode(EN_I2S_DAC, OUTPUT);
  // digitalWrite(EN_I2S_DAC, LOW);
  digitalWrite(EN_I2S_DAC, HIGH);
  // init display
  pinMode(EN_OLED_PIN, OUTPUT);
  digitalWrite(EN_OLED_PIN, HIGH);
  displayManager::initOLED(&display, 90);
  displayManager::setTitle(menuTxt, sizeof(menuTxt) / sizeof(menuTxt[0]),
                           wearerIdTxt,
                           sizeof(wearerIdTxt) / sizeof(wearerIdTxt[0]));
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
  displayManager::updateOLED(&display, audioManager::getPlayCategory(),
                             audioManager::getGain());
  // set device position as NECK
  audioManager::setDevicePos(0);
  // audioManager::setDevicePos(5);

  audioManager::setWearerId(wearerId);
#endif

// v1_2に移行したら消す
#ifdef NECKLACE_V1
  pinMode(G_SEL_B_PIN, OUTPUT);
  pinMode(G_SEL_A_PIN, OUTPUT);
  // ボタンピン設定
  for (int i = 0; i < sizeof(SW_PIN) / sizeof(SW_PIN[0]); i++) {
    pinMode(SW_PIN[i], INPUT);
  };
  // Show LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, 1);
  // leds[0] = CRGB::Black;
  leds[0].setRGB(5, 5, 5);
  FastLED.show();
  // EN_OLED_PIN High
  pinMode(EN_OLED_PIN, OUTPUT);
  digitalWrite(EN_OLED_PIN, HIGH);

  // MAX初期設定
  // MAX用、LR設定用の電圧
  pinMode(SDMODE_PIN, OUTPUT);
  digitalWrite(SDMODE_PIN, HIGH);
  pinMode(G_SEL_B_PIN, OUTPUT);
  pinMode(G_SEL_A_PIN, OUTPUT);
  audioManager::setGain(G_SEL_A_PIN, G_SEL_B_PIN, 2);
  // init display
  displayManager::initOLED(&display, 90);
  displayManager::setTitle(menuTxt, sizeof(menuTxt) / sizeof(menuTxt[0]),
                           gainTxt, sizeof(gainTxt) / sizeof(gainTxt[0]));
  displayManager::updateOLED(&display, audioManager::getPlayCategory(),
                             audioManager::getGain());

  // set device position as NECK
  audioManager::setDevicePos(0);
  audioManager::setWearerId(0);
#endif

#ifdef GENERAL
  pinMode(G_SEL_B_PIN, OUTPUT);
  pinMode(G_SEL_A_PIN, OUTPUT);
  pinMode(SW1_PIN, INPUT);
  pinMode(SW2_PIN, INPUT);
  pinMode(SW3_PIN, INPUT);
  // Show LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, 4);
  for (int i = 0; i < 4; i++) {
    leds[i] = CRGB::Black;
  }
  pinMode(SD_MODE_PIN, OUTPUT);
  digitalWrite(SD_MODE_PIN, HIGH);
  // MAX初期設定
  gainNum = 2;
  audioManager::setGain(G_SEL_A_PIN, G_SEL_B_PIN, 2);
  audioManager::setDevicePos(5);
  audioManager::setPlayCategory(0);
  wearerId = 0;
  audioManager::setWearerId(wearerId);
  leds[wearerId].setRGB(5, 5, 5);
  FastLED.show();
#endif
  // flash memory 内のファイルを読SW2_SEL_P_PINみ込み
  audioManager::readAllWavFiles();
  audioManager::initAudioOut(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);
  // espnowの開始
  espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);

  xTaskCreatePinnedToCore(TaskUI, "TaskUI", 2048, NULL, 1, &thp[0], 1);
  xTaskCreatePinnedToCore(TaskAudio, "TaskAudio", 2048, NULL, 2, &thp[1], 1);
}

void loop() {}