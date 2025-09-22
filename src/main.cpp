#include "globals.h"

//////////////// general functions  /////////////////////////////

void TaskAudio(void *args) {
  while (1) {
    audioManager::playAudioInLoop();
    delay(5);
  }
}

/////////////// execution ////////////////////////////////////
void setup() {
  // デバッグ用ボタン
  USBSerial.begin(115200);
  USBSerial.println("init Hapbeat wireless reciever");
  audioManager::initParamsEEPROM();
  delay(5);
  // ボタンピン設定
  for (int i = 0; i < sizeof(_SW_PIN) / sizeof(_SW_PIN[0]); i++) {
    pinMode(_SW_PIN[i], INPUT_PULLUP);  // プルアップ抵抗を有効化
  };
  // Show LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(_leds, 1);
  // init I2S DAC
  pinMode(EN_I2S_DAC_PIN, OUTPUT);
  digitalWrite(EN_I2S_DAC_PIN, HIGH);
  // init _display
  pinMode(EN_OLED_PIN, OUTPUT);
  digitalWrite(EN_OLED_PIN, HIGH);
  displayManager::initOLED(&_display, DISP_ROT, FONT_SIZE, CATEGORY_TEXT_POS,
                           CHANNEL_TEXT_POS, GAIN_STEP_TEXT_POS);
  const char *initMsg = "Initializing...";
  displayManager::printEfont(&_display, initMsg, 0, 8);
  // vibAmp
  pinMode(EN_VIBAMP_PIN, OUTPUT);
  digitalWrite(EN_VIBAMP_PIN, HIGH);
  pinMode(AOUT_VIBVOL_PIN, OUTPUT);
  // D級アンプの初期設定＆ゲイン決定
  pinMode(EN_MOTOR_PIN, OUTPUT);
  digitalWrite(EN_MOTOR_PIN, HIGH);
  pinMode(BQ27x_PIN, INPUT);
  audioManager::setDevicePos(DEVICE_POS);

#if defined(NECKLACE_V2) || defined(NECKLACE_V3)
  // battery current sensing pins
  pinMode(BAT_CURRENT_PIN, INPUT);
  pinMode(DETECT_ANALOG_IN_PIN, INPUT);
  pinMode(AIN_VIBVOL_PIN, INPUT);
#endif

#if defined(BAND_V3)
  // CATEGORY_ID_TXT_SIZEをセット
  audioManager::setCategorySize(CATEGORY_ID_TXT_SIZE);
  // ボリュームレベル配列を初期化
  uint8_t volumeLevels[CATEGORY_ID_TXT_SIZE] = {0};
  audioManager::loadVolumeLevels(volumeLevels, CATEGORY_ID_TXT_SIZE);
#endif

  // I2C関連 init
  // SDA_PIN と SCL_PIN を明示する。
  Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C master
#if defined(BAND_V3)
  // Band_V3: BQ27220 初期化（容量設定は行わない）
  BQ27220_Cmd::setupBQ27220(SDA_PIN, SCL_PIN, 0);
#endif
#ifdef EN_MCP4018
  // 以下の begin の中に Wire.begin() があるが、引数が無いので SDA_PIN と
  // SDA_PIN を明示できない。事前に Wire.begin(SDA_PIN, SDA_PIN) が必要
  _digipot.begin();  // Initialize Digipot library.
  _digipot.setWiperPercent(0);
#endif
  USBSerial.println("I2C connected");
  // データ読み込み
  audioManager::readAllSoundFiles();
  audioManager::initAudioOut(BCLK_PIN, LRCK_PIN, DOUT_PIN);
  // 読み込む前に playSndOnRecv 入るとエラーになるので、読み込むための時間を確保
  delay(200);

  _isFixMode = audioManager::getIsFixMode();
  if (_isFixMode) {
    _currentColor = COLOR_FIX_MODE;
  } else {
    _currentColor = COLOR_VOL_MODE;
  }
  _leds[0] = _currentColor;
  FastLED.show();

  TaskAppInit();
  
  xTaskCreatePinnedToCore(TaskAudio, "TaskAudio", 4096, NULL, 20, &thp[0], 1);
  TaskAppStart();
}
void loop() {
  TaskAppLoop();
};

// 優先度は 低 0-24 高
// BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode,
//                                    const char *constpcName,
//                                    const uint32_t usStackDepth,
//                                    void *constpvParameters,
//                                    UBaseType_t uxPriority,
//                                    TaskHandle_t *constpvCreatedTask,
//                                    const BaseType_t xCoreID)
