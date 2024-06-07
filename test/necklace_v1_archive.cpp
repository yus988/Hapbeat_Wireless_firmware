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

void setup() {
  // デバッグ用ボタン
  USBSerial.begin(115200);
  USBSerial.println("init Hapbeat wireless reciever");

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
  displayManager::initOLED(&display, DISP_ROT);
  displayManager::setTitle(menuTxt, sizeof(menuTxt) / sizeof(menuTxt[0]),
                           gainTxt, sizeof(gainTxt) / sizeof(gainTxt[0]));
  displayManager::updateOLED(&display, audioManager::getPlayCategory(),
                             audioManager::getGain());

  // set device position as NECK
  audioManager::setDevicePos(0);
  audioManager::setWearerId(0);
#endif


  // flash memory 内のファイルを読SW2_SEL_P_PINみ込み
  readAllWavFiles();
  initAudioOut(I2S_BCLK_PIN, I2S_LRCK_PIN, I2S_DOUT_PIN);
  // espnowの開始
  init_esp_now(PlaySndOnDataRecv);
}

void loop() {
// v1_2に移行したら消す
#ifdef NECKLACE_V1
  for (int i = 0; i < sizeof(SW_PIN) / sizeof(SW_PIN[0]); i++) {
    if (!digitalRead(SW_PIN[i]) && !isBtnPressed[i]) {
      uint8_t playCategoryNum = getPlayCategory();
      uint8_t gainNum = getGain();
      stopAll();
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
      updateOLED(&display, playCategoryNum, gainNum);
      isBtnPressed[i] = true;
      setGain(G_SEL_A_PIN, G_SEL_B_PIN, gainNum);
      setPlayCategory(playCategoryNum);
      delay(1);
    }
    if (digitalRead(SW_PIN[i]) && isBtnPressed[i]) isBtnPressed[i] = false;
  };
#endif

  playAudioInLoop();
}

[env:necklace_v1] lib_deps =
    earlephilhower / ESP8266Audio @^1.9.7 fastled / FastLED @^3.6.0 adafruit /
    Adafruit SSD1306 @^2.5.9 tanakamasayuki /
    efont Unicode Font Data @^1.0.9 build_flags = -D NECKLACE_V1

    