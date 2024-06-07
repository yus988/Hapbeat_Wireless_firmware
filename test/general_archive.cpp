#include <Arduino.h>
#include "FS.h"
#include <LittleFS.h>
// オーディオ再生関連 ESP8266
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioOutputMixer.h"
#include "driver/i2s.h"
// 無線関連
#include <esp_now.h>
#include <WiFi.h>
// ディスプレイ関連
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "efontSSD1306.h"
// LED
#include <FastLED.h>

// //////////////// play audio begin ///////////////////////
bool isPlayAudio = false;
uint8_t playChannel = 0;
// pins related to audio
#define I2S_BCLK 39
#define I2S_DOUT 40
#define I2S_LRCK 38
#define SDMODE_PIN 21  // for MAX98357A
#define G_SEL_A 41
#define G_SEL_B 42
#define G_SEL_C 47

#define MAX_VOL 255  // for volume control

// M5StickS3
// #define SW3_SEL_P 9  // デバッグ用ボタンのピン番号
// #define I2S_BCLK 1
// #define I2S_DOUT 3
// #define I2S_LRCK 5
AudioGeneratorWAV *wav_gen[2];
AudioFileSourcePROGMEM *src_prog[2];
AudioFileSourceLittleFS *file;
AudioOutputI2S *i2s_out;
AudioOutputMixer *mixer;
AudioOutputMixerStub *stub[2];
// [カテゴリ、ポジション、id、左右]
uint8_t *audioData[9][9][9][2];
size_t audioDataSize[9][9][9][2];

// ファイルの読み込み
void readAllWavFiles() {
  u_int8_t i;
  u_int8_t cat;  // file name
  u_int8_t pos;
  u_int8_t id;
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    cat = file.name()[0] - '0';
    pos = file.name()[1] - '0';
    id = file.name()[2] - '0';
    // 左右の弁別、先頭から4文字目がRの場合、1、それ以外（L or C）は0
    i = file.name()[3] != 'R' ? 0 : 1;
    Serial.printf("FILE: %s, %d, %d, %d, %c, i: %d\n", file.name(), cat, pos,
                  id, file.name()[3], i);
    audioDataSize[cat][pos][id][i] = file.size();
    audioData[cat][pos][id][i] = new uint8_t[audioDataSize[cat][pos][id][i]];
    file.read(audioData[cat][pos][id][i], audioDataSize[cat][pos][id][i]);
    file = root.openNextFile();
  }
  // init audio
  src_prog[0] = new AudioFileSourcePROGMEM();
  src_prog[1] = new AudioFileSourcePROGMEM();
  i2s_out = new AudioOutputI2S();
  i2s_out->SetPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
  wav_gen[0] = new AudioGeneratorWAV();
  wav_gen[1] = new AudioGeneratorWAV();
  // mixer に１回渡したら、あとは stub => mixer の順で色々渡す
  mixer = new AudioOutputMixer(32, i2s_out);
  stub[0] = mixer->NewInput();
  stub[1] = mixer->NewInput();
}

void stopAll() {
  wav_gen[0]->stop();
  wav_gen[1]->stop();
  stub[0]->stop();
  stub[1]->stop();
  isPlayAudio = false;
}

//////////////// exp_now begin //////////////////////////
esp_now_peer_info_t slave;
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // 割り込みの場合、再生中のものを停止
  stopAll();
  // data = [category, pos, id, L_Vol, R_Vol ]
  uint8_t cat = data[0];
  uint8_t pos = data[1];
  uint8_t id = data[2];
  uint8_t isStereo = data[3];
  uint8_t L_Vol = data[4];
  uint8_t R_Vol = data[5];
  Serial.printf("recieced: id %d, isStereo %d, Vol %d:%d, \n", id, isStereo,
                L_Vol, R_Vol);
  // 表示チャンネルと受信チャンネルが合致した時に再生
  if (playChannel == cat) {
    // start L
    src_prog[0]->open(audioData[cat][pos][id][0],
                      audioDataSize[cat][pos][id][0]);
    stub[0]->SetGain((float)L_Vol / MAX_VOL);
    wav_gen[0]->begin(src_prog[0], stub[0]);
    if (isStereo) {
      // start R
      src_prog[1]->open(audioData[cat][pos][id][1],
                        audioDataSize[cat][pos][id][1]);
      stub[1]->SetGain((float)R_Vol / MAX_VOL);
      wav_gen[1]->begin(src_prog[1], stub[1]);
    }
    isPlayAudio = true;
  }
}

void init_esp_now() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
  // マルチキャスト用Slave登録
  memset(&slave, 0, sizeof(slave));
  for (int i = 0; i < 6; ++i) {
    slave.peer_addr[i] = (uint8_t)0xff;
  }
  esp_err_t addStatus = esp_now_add_peer(&slave);
  if (addStatus == ESP_OK) {  // Pair success
    Serial.println("Pair success");
  }
  // ESP-NOWコールバック登録
  esp_now_register_recv_cb(OnDataRecv);
}

// ////////// UI ///////////////////////////////////////////
// ESP-WROOM-32, 12,13だと書き込めなくなる？
// I2C 未使用
#define SCL_PIN 5
#define SDA_PIN 6
// Display pins
// #define SCLK_PIN 15
// #define MOSI_PIN 16
// #define MISO_PIN 1
// #define OLED_DC 3
// #define OLED_RESET 4
#define SCLK_PIN 35
#define MOSI_PIN 36
#define MISO_PIN 37
#define OLED_DC 34
#define OLED_RESET 33  // Reset pin # (or -1 if sharing Arduino reset pin)
#define CS_PIN 27
#define EN_OLED 14
// Button pins
#define SW1_VOL_P 13
#define SW2_VOL_N 12
#define SW3_SEL_P 11
#define SW4_SEL_N 10
#define SW5_ENTER 9
// LED
#define LED_PIN 2
// params
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, MOSI_PIN, SCLK_PIN,
                         OLED_DC, OLED_RESET, CS_PIN);
// // LED
CRGB leds[1];

bool isBtnPressed[2] = {false, false};  // control state of being pressed
const char *menuTxt[3] = {"テニス", "野球", "バスケ"};
const char *gainTxt[5] = {"1", "2", "3", "4", "5"};
uint8_t gainNum = 0;

void updateOLED(uint8_t ch, uint8_t gain) {
  display.clearDisplay();
  // チャンネルの更新
  printEfont(&display, menuTxt[ch], 0, 8);
  // ボリュームの更新
  printEfont(&display, gainTxt[gain], 128 - 16, 8);
  display.display();
}

/////////////// execution ////////////////////////////////////
void setup() {
  // // デバッグ用ボタン
  Serial.begin(115200);
  Serial.println("init Hapbeat wireless reciever");
  // ピン設定
  pinMode(SW1_VOL_P, INPUT);
  pinMode(SW2_VOL_N, INPUT);
  pinMode(SW3_SEL_P, INPUT);
  pinMode(SW4_SEL_N, INPUT);
  pinMode(SW5_ENTER, INPUT);
  // MAX用、LR設定用の電圧
  pinMode(SDMODE_PIN, OUTPUT);
  digitalWrite(SDMODE_PIN, HIGH);

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, 1);
  // leds[0] = CRGB::Black;
  leds[0].setRGB(5, 5, 5);
  FastLED.show();

  // // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // pinMode(EN_OLED, OUTPUT);
  // digitalWrite(EN_OLED, HIGH);
  // if (!display.begin(SSD1306_SWITCHCAPVCC)) {
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for (;;)
  //     ;  // Don't proceed, loop forever
  // }
  // display.clearDisplay();
  // display.setRotation(2);
  // updateOLED(playChannel, gainNum);
  // display.display();

  // LittleFSマウント確認
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  // espnowの開始
  init_esp_now();
  // flash memory 内のファイルを読み込み
  readAllWavFiles();
}

void loop() {

  
  //  Serial.print("Hapbeat");
  // ボタン操作。将来的に別のコアで動作させたい
  // チャンネル変更
  if (!digitalRead(SW3_SEL_P) && !isBtnPressed[0]) {
    // stopAll();
    // if (playChannel < 2)  // 配列数-1
    //   playChannel += 1;
    // else
    //   playChannel = 0;
    // updateOLED(playChannel, gainNum);
    isBtnPressed[0] = true;
    Serial.println("SELECTED");
  }
  if (digitalRead(SW3_SEL_P) && isBtnPressed[0]) isBtnPressed[0] = false;
  // // ゲイン変更
  // if (!digitalRead(SW1_VOL_P) && !isBtnPressed[1]) {
  //   stopAll();
  //   if (gainNum < 4)  // 配列数-1
  //     gainNum += 1;
  //   else
  //     gainNum = 0;
  //   updateOLED(playChannel, gainNum);
  //   isBtnPressed[1] = true;
  //   Serial.println("VOLUP");
  // }
  // if (digitalRead(SW1_VOL_P) && isBtnPressed[1]) isBtnPressed[1] = false;

  // オーディオ再生部
  while (isPlayAudio == true) {
    if (wav_gen[0]->isRunning()) {
      if (!wav_gen[0]->loop()) stopAll();
    }
    if (wav_gen[1]->isRunning()) {
      if (!wav_gen[1]->loop()) stopAll();
    }
  }
}
