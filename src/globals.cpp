// 保持用変数と頻繁に調整しない変数をmain.cppとtask関数内で使用するグローバル変数として定義
#include "globals.h"
#include "adjustParams.h"  // LIMITED_IDS および DISP_MSG の型とサイズを取得

const float SHUNT_RESISTANCE = 0.01;  // シャント抵抗 (オーム)
const float INA_GAIN = 50.0;          // INA180A2IDBVRのゲイン (V/V)
const int ADC_MAX = 4095;             // ADCの分解能
const float V_REF = 3.3;              // アナログ基準電圧 (V)
const int BATTERY_CAPACITY = 3000;    // バッテリー容量 (mAh)

TaskHandle_t thp[3];  // 環境によって必要タスク数が変わるので注意
bool _isFixMode;
unsigned long _lastDisplayUpdate =
    0;  // ディスプレイが最後に更新された時刻を保持
unsigned long _lastBatStatusUpdate = 0;
// LED
CRGB _leds[1];
CRGB _currentColor;
const int SCREEN_WIDTH = 128;  // OLED _display width, in pixels
const int SCREEN_HEIGHT = 32;  // OLED _display height, in pixels
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                          OLED_DC_PIN, OLED_RESET_PIN, CS_PIN);

//////////////////////////// general functions
////////////////////////////////////////

// 所定の値に固定する。
void setFixGain(bool updateOLED) {
  int fixVolume = FIX_GAIN_STEP[audioManager::getPlayCategory()];
#if defined(NECKLACE_V3)
  int volume = map(fixVolume, 0, GAIN_STEP_TXT_SIZE - 1, 0, 100);
  _digipot.setWiperPercent(volume);
#elif defined(NECKLACE_V2) || defined(BAND_V2)
  int volume = map(fixVolume, 0, GAIN_STEP_TXT_SIZE - 1, 0, 255);
  analogWrite(AOUT_VIBVOL_PIN, volume);
#endif
  if (updateOLED) {
    displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                               audioManager::getWearerId(),
                               FIX_GAIN_STEP[audioManager::getPlayCategory()]);
  }
}

// PAMの電圧を下げる
void setAmpStepGain(int step, bool updateOLED) {
#ifdef NECKLACE_V3
  int volume = map(step, 0, GAIN_STEP_TXT_SIZE - 1, 0, 100);
  _digipot.setWiperPercent(volume);
#elif NECKLACE_V2
  int volume = map(_currAIN, 0, 4095, 0, 255);
  analogWrite(AOUT_VIBVOL_PIN, volume);
#endif
  // ディスプレイにdB表示用のステップ数変換
  if (updateOLED) {
    displayManager::updateOLED(&_display, audioManager::getPlayCategory(),
                               audioManager::getWearerId(), step);
  }
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
  _lastDisplayUpdate = millis();  // 画面更新時刻をリセット
}

#if defined(NECKLACE_V2)
// see pam8003 datasheet p.7
int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN, SW3_SEL_P_PIN, SW4_SEL_N_PIN,
                 SW5_ENTER_PIN};
bool _isBtnPressed[] = {false, false, false, false, false};

// volume related variables
int _prevAIN = 0;
int _currAIN = 0;
uint8_t _ampVolStep;
#endif

#if defined(NECKLACE_V3)
MCP4018_SOLDERED _digipot;  // オブジェクトの定義
#endif

#if defined(BAND_V2)
// see pam8003 datasheet p.7
int _SW_PIN[] = {SW1_VOL_P_PIN, SW2_VOL_N_PIN};
bool _isBtnPressed[] = {false, false};
#endif