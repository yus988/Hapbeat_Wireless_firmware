// 頻繁に調整したい変数はこちらで調整
#include "adjustParams.h"

#ifdef NECKLACE_V2
const int volumeThreshold = 5;
const char *PLAY_CATEGORY_TXT[] = {"ゲーム", "動画", "hoge"};
const int FIX_GAIN_STEP[] = {21, 16, 0};  // 各 playCategory に対応
const char *WEARER_ID_TXT[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB COLOR_FIX_MODE = CRGB(10, 10, 10);
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 10);
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);
const char *DECIBEL_TXT[] = {
    "-75",  "-34",  "-22",  "-10",  "-5",   "0",    "3.0",  "4.4",
    "5.2",  "6.0",  "6.8",  "7.6",  "8.4",  "9.2",  "10.0", "10.8",
    "11.6", "12.4", "13.2", "14.0", "14.8", "15.6", "16.4", "17.2",
    "18.0", "18.8", "19.6", "20.4", "21.2", "22.0", "22.8", "24.0"};
const int DISP_ROT = 0;  // ディスプレイの回転設定 左利き用
#endif

#ifdef BAND_V2

const int DISP_ROT = 90;                  // 右利き用
const int FIX_GAIN_STEP[] = {21, 15, 0};  // 各 playCategory に対応 0--63
const char *PLAY_CATEGORY_TXT[] = {"ゲーム", "動画", "hoge"};
const char *WEARER_ID_TXT[] = {"ALL", "競技", "会場", "4", "5", "6"};
const CRGB COLOR_FIX_MODE = CRGB(0, 0, 2);  // 無制限
const CRGB COLOR_VOL_MODE = CRGB(2, 0, 0);  // 制限
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);
const char *DECIBEL_TXT[] = {
    "-75",  "-34",  "-22",  "-10",  "-5",   "0",    "3.0",  "4.4",
    "5.2",  "6.0",  "6.8",  "7.6",  "8.4",  "9.2",  "10.0", "10.8",
    "11.6", "12.4", "13.2", "14.0", "14.8", "15.6", "16.4", "17.2",
    "18.0", "18.8", "19.6", "20.4", "21.2", "22.0", "22.8", "24.0"};
const int BAT_NOTIFY_SOC = 5;               // 残量低下を通知する閾値（％）
const int BAT_NOTIFY_VOL = 3600;            // 残量低下を通知する閾値（mV）
const unsigned int DISPLAY_TIMEOUT = 3000;  // 3000ミリ秒
const unsigned int BATTERY_STATUS_INTERVAL = 30000;  // 30000ミリ秒

  #ifdef MQTT
const ID_definitions ID_MSG = {0, 1, 2, 3};  // 例
const MessageData DISP_MSG[] = {
    {0, "青 / Blue \nランプ点灯"},
    {1, "黄 / Yellow \nランプ点灯"},
    {2, "赤 / Red \nランプ点灯"},
    {3, "メッセージ 4"},
};
const int LIMITED_IDS[] = {0, 1};  // 制限非制限
const char *LIMIT_ENABLE_MSG[] = {
    "全て再生", "制限モード"};  // 0: isLimitEnable = false, 1: true
  #endif

#endif

// 各配列のサイズを定義
// ESPNOWで使用
#ifdef ESPNOW
const int PLAY_CATEGORY_TXT_SIZE =
    sizeof(PLAY_CATEGORY_TXT) / sizeof(PLAY_CATEGORY_TXT[0]);
const int WEARER_ID_TXT_SIZE = sizeof(WEARER_ID_TXT) / sizeof(WEARER_ID_TXT[0]);
const int DECIBEL_TXT_SIZE = sizeof(DECIBEL_TXT) / sizeof(DECIBEL_TXT[0]);
#elif MQTT
// MQTTで使用
const int LIMITED_IDS_SIZE =
    sizeof(LIMITED_IDS) / sizeof(LIMITED_IDS[0]);                  // サイズ定義
const int DISP_MSG_SIZE = sizeof(DISP_MSG) / sizeof(DISP_MSG[0]);  // サイズ定義
#endif