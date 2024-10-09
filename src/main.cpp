#include "globals.h"

//////////////// general functions  /////////////////////////////

void TaskAudio(void *args) {
  while (1) {
    audioManager::playAudioInLoop();
    delay(50);
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

#endif

#if defined(GENERAL_V2)
  audioManager::setDevicePos(5);
#endif

#ifdef ESPNOW
  // ここはタスク依存
  USBSerial.println("setup before setTitile");
  displayManager::setTitle(PLAY_CATEGORY_TXT, PLAY_CATEGORY_TXT_SIZE,
                           WEARER_ID_TXT, WEARER_ID_TXT_SIZE, DECIBEL_TXT,
                           DECIBEL_TXT_SIZE);
  USBSerial.println("setup after setTitile");
  setFixGain(true);  // 実行しないと VibAmpVolume = 0 のままなので必須
  USBSerial.println("setup after setFixGain");
  // espnowManager::init_esp_now(audioManager::PlaySndOnDataRecv);
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
  while (!MQTT_manager::getIsWiFiConnected()) {
    USBSerial.println("waiting for WiFi connection...");
    delay(500);  // 少し待って再試行
  };
#endif
  audioManager::readAllSoundFiles();
  audioManager::initAudioOut(BCLK_PIN, LRCK_PIN, DOUT_PIN);
  BQ27220_Cmd::setupBQ27220(SDA_PIN, SCL_PIN, BATTERY_CAPACITY);
  xTaskCreatePinnedToCore(TaskAudio, "TaskAudio", 2048, NULL, 20, &thp[1], 1);
  xTaskCreatePinnedToCore(TaskUI, "TaskUI", 2048, NULL, 23, &thp[0], 1);

#ifdef NECKLACE_V_1_4
  _digipot.begin();  // Initialize Digipot library.
  _digipot.setWiperPercent(50);
  USBSerial.print("Digipot wiper: ");
  USBSerial.print(_digipot.getWiperValue(), DEC);
  USBSerial.println('%');
#endif
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
