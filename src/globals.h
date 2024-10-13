#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
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
  #include "taskESPNOW.h"
#elif MQTT
  #include <MQTT_manager.h>
  #include "taskMQTT.h"
#endif

#ifdef NECKLACE_V3
  #include "MCP4018-SOLDERED.h"  // Include Soldered library for MCP4018 Digipot.
extern MCP4018_SOLDERED _digipot;  // Create object for Digipot library.
#endif

#include "adjustParams.h"
#include "pinAssign.h"

// 定数の宣言
extern const float SHUNT_RESISTANCE;  // シャント抵抗 (オーム)
extern const float INA_GAIN;          // INA180A2IDBVRのゲイン (V/V)
extern const int ADC_MAX;             // ADCの分解能
extern const float V_REF;             // アナログ基準電圧 (V)
extern const int BATTERY_CAPACITY;    // バッテリー容量 (mAh)

// タスク関連の変数
extern TaskHandle_t thp[3];  // タスクハンドル
extern bool _isFixMode;
extern unsigned long _lastDisplayUpdate;    // ディスプレイ更新時刻
extern unsigned long _lastBatStatusUpdate;  // バッテリー状態更新時刻

// LED関連の変数
extern CRGB _leds[1];
extern CRGB _currentColor;

// OLEDディスプレイ関連の変数
extern const int SCREEN_WIDTH;   // OLED _display width
extern const int SCREEN_HEIGHT;  // OLED _display height
extern Adafruit_SSD1306 _display;

// バージョンごとのスイッチとボタンの状態
#if defined(NECKLACE_V2)
extern int _SW_PIN[5];
extern bool _isBtnPressed[5];

// 音量関連の変数
extern int _prevAIN;
extern int _currAIN;
extern uint8_t _ampVolStep;
#endif

#if defined(GENERAL_V2)
extern int _SW_PIN[2];
extern bool _isBtnPressed[2];
#endif

// 電流計算の関数宣言
float calculateCurrent(int adc_value);
void setFixGain(bool updateOLED = true);
void setAmpStepGain(int step, bool updateOLED = true);
#endif  // GLOBALS_H
