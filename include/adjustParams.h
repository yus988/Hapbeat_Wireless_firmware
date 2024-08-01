// 頻繁に調整したい変数はこちらで定義

#ifndef ADJ_PARAMS_H
#define ADJ_PARAMS_H

#include <Adafruit_SSD1306.h>
#if defined(NECKLACE)
const int FIX_GAIN_STEP = 41;
const char *PLAY_CATEGORY_TXT[] = {"ゲーム", "動画", "hoge"};
const char *WEARER_ID_TXT[] = {"ALL", "競技", "会場", "4", "5", "6"};
const char *DECIBEL_TXT[] = {
    "-75",  "-40",  "-34",  "-28",  "-22",  "-16",  "-10",  "-7.5",
    "-5",   "-2.5", "0",    "1.5",  "3.0",  "4.0",  "4.4",  "4.8",
    "5.2",  "5.6",  "6.0",  "6.4",  "6.8",  "7.2",  "7.6",  "8.0",
    "8.4",  "8.8",  "9.2",  "9.6",  "10.0", "10.4", "10.8", "11.2",
    "11.6", "12.0", "12.4", "12.8", "13.2", "13.6", "14.0", "14.4",
    "14.8", "15.2", "15.6", "16.0", "16.4", "16.8", "17.2", "17.6",
    "18.0", "18.4", "18.8", "19.2", "19.6", "20.0", "20.4", "20.8",
    "21.2", "21.6", "22.0", "22.4", "22.8", "23.2", "23.6", "24.0"};
#endif

#if defined(NECKLACE_V_1_3)
const int volumeThreshold = 100;
const char *PLAY_CATEGORY_TXT[] = {"ゲーム", "動画", "hoge"};
const int FIX_GAIN_STEP[] = {41, 28, 0};  // 各 playCategory に対応
const char *WEARER_ID_TXT[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB COLOR_FIX_MODE = CRGB(10, 10, 10);
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 10);
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);
const char *DECIBEL_TXT[] = {
    "-75",  "-40",  "-34",  "-28",  "-22",  "-16",  "-10",  "-7.5",
    "-5",   "-2.5", "0",    "1.5",  "3.0",  "4.0",  "4.4",  "4.8",
    "5.2",  "5.6",  "6.0",  "6.4",  "6.8",  "7.2",  "7.6",  "8.0",
    "8.4",  "8.8",  "9.2",  "9.6",  "10.0", "10.4", "10.8", "11.2",
    "11.6", "12.0", "12.4", "12.8", "13.2", "13.6", "14.0", "14.4",
    "14.8", "15.2", "15.6", "16.0", "16.4", "16.8", "17.2", "17.6",
    "18.0", "18.4", "18.8", "19.2", "19.6", "20.0", "20.4", "20.8",
    "21.2", "21.6", "22.0", "22.4", "22.8", "23.2", "23.6", "24.0"};
// シャットダウン回避関連
// 新バッテリー用（最大3.8A程度）
// const float current_thresholds[2] = {5.0, 5.5};  // 電流値の閾値 (A)
// const int shutdownCycles[2] = {20, 4};           //
// シャットダウンサイクル数。大きいほど、長時間閾値越えを許容する。閾値に応じて変える
// 旧バッテリー用（最大 4.1A 程度）
const float current_thresholds[2] = {5.2, 5.5};  // 電流値の閾値 (A)
const int shutdownCycles[2] = {100, 40};
// シャットダウンサイクル数。大きいほど、長時間閾値越えを許容する。閾値に応じて変える
const int restoreCycles = 100;  // 復帰サイクル数
#endif
#if defined(GENERAL_V2)

// const int DISP_ROT = 0; // 左利き用
const int DISP_ROT = 90; // 右利き用

const int FIX_GAIN_STEP[] = {41, 30, 0};  // 各 playCategory に対応 0--63
const char *PLAY_CATEGORY_TXT[] = {"ゲーム", "動画", "hoge"};
const char *WEARER_ID_TXT[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB COLOR_FIX_MODE = CRGB(0, 0, 2);  // 無制限
const CRGB COLOR_VOL_MODE = CRGB(2, 0, 0);  // 制限
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);
const int LIMITED_IDS[] = {0, 1};
const char *LIMIT_ENABLE_MSG[] = {
    "全て再生", "制限モード"};  // 0: isLimitEnable = false, 1: true

const char *DECIBEL_TXT[] = {
    "-75",  "-40",  "-34",  "-28",  "-22",  "-16",  "-10",  "-7.5",
    "-5",   "-2.5", "0",    "1.5",  "3.0",  "4.0",  "4.4",  "4.8",
    "5.2",  "5.6",  "6.0",  "6.4",  "6.8",  "7.2",  "7.6",  "8.0",
    "8.4",  "8.8",  "9.2",  "9.6",  "10.0", "10.4", "10.8", "11.2",
    "11.6", "12.0", "12.4", "12.8", "13.2", "13.6", "14.0", "14.4",
    "14.8", "15.2", "15.6", "16.0", "16.4", "16.8", "17.2", "17.6",
    "18.0", "18.4", "18.8", "19.2", "19.6", "20.0", "20.4", "20.8",
    "21.2", "21.6", "22.0", "22.4", "22.8", "23.2", "23.6", "24.0"};
const int BAT_NOTIFY_SOC = 5;     // 残量低下を通知する閾値（％）
const int BAT_NOTIFY_VOL = 3600;  // 残量低下を通知する閾値（mV）

const unsigned int DISPLAY_TIMEOUT =
    5000;  // ディスプレイを消灯するまでの時間（msec）

const unsigned int BATTERY_STATUS_INTERVAL =
    10000;  // バッテリー情報を更新する時間（msec）

// 各通知と音源のIDを紐づけ（メッセージの判別などに使用）
struct ID_definitions {
  uint8_t blue;
  uint8_t yellow;
  uint8_t red;
  uint8_t notify;  // 電池残量減少
};
const ID_definitions ID_MSG = {.blue = 0, .yellow = 1, .red = 2, .notify = 3};

// ディスプレイに表示させるメッセージを管理
struct MessageData {
  uint8_t id;
  const char *message;
};
// メッセージとIDの初期化
const MessageData DISP_MSG[] = {{ID_MSG.blue, "Blue signal\n occured"},
                                {ID_MSG.yellow, "Yellow signal\n occured"},
                                {ID_MSG.red, "Red signal\n occured"}};

#endif

#endif