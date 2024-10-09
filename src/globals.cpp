// 保持用変数と頻繁に調整しない変数をmain.cppとtask関数内で使用するグローバル変数として定義
#include "globals.h"

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
const int SCREEN_WIDTH = 128;  // OLED _display width, in pixels
const int SCREEN_HEIGHT = 32;  // OLED _display height, in pixels
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                          OLED_DC_PIN, OLED_RESET_PIN, CS_PIN);


// 所定の値に固定する。
void setFixGain(bool updateOLED) {
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
#endif

#if defined(NECKLACE_V_1_4)
MCP4018_SOLDERED _digipot; // オブジェクトの定義
#endif


#if defined(GENERAL_V2)
// see pam8003 datasheet p.7
int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN};
bool _isBtnPressed[] = {false, false};
#endif