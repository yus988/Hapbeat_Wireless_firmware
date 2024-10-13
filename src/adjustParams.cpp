// 頻繁に調整したい変数はこちらで調整
#include "adjustParams.h"

#ifdef NECKLACE
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

#ifdef NECKLACE_V2
const int volumeThreshold = 5;
const char *PLAY_CATEGORY_TXT[] = {"ゲーム", "動画", "hoge"};
const int FIX_GAIN_STEP[] = {41, 24, 0};  // 各 playCategory に対応
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

const float current_thresholds[2] = {5.2, 5.5};  // 電流値の閾値 (A)
const int shutdownCycles[2] = {100, 40};
const int restoreCycles = 100;    // 復帰サイクル数
const int BAT_NOTIFY_SOC = 5;     // 残量低下を通知する閾値（％）
const int BAT_NOTIFY_VOL = 3600;  // 残量低下を通知する閾値（mV）
const int DISP_ROT = 0;  // ディスプレイの回転設定 左利き用
#endif

#ifdef GENERAL_V2
const int DISP_ROT = 90;                  // 右利き用
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
const unsigned int DISPLAY_TIMEOUT = 3000;           // 3000ミリ秒
const unsigned int BATTERY_STATUS_INTERVAL = 30000;  // 30000ミリ秒

struct ID_definitions {
  uint8_t blue;
  uint8_t yellow;
  uint8_t red;
  uint8_t notify;  // 電池残量減少
};

const ID_definitions ID_MSG = {0, 1, 2, 3};  // 例

struct MessageData {
  uint8_t id;
  const char *message;
};

const MessageData DISP_MSG[] = {
    {0, "メッセージ 1"},
    {1, "メッセージ 2"},
    {2, "メッセージ 3"},
    {3, "メッセージ 4"},
};
#endif

// 各配列のサイズを定義
const int PLAY_CATEGORY_TXT_SIZE =
    sizeof(PLAY_CATEGORY_TXT) / sizeof(PLAY_CATEGORY_TXT[0]);
const int WEARER_ID_TXT_SIZE = sizeof(WEARER_ID_TXT) / sizeof(WEARER_ID_TXT[0]);
const int DECIBEL_TXT_SIZE = sizeof(DECIBEL_TXT) / sizeof(DECIBEL_TXT[0]);
