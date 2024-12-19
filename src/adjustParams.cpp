// 頻繁に調整したい変数はこちらで調整
#include "adjustParams.h"

/* カテゴリと装着者やチャンネルの意図について

- PLAY_CATEGORY_TXT（カテゴリ名の設定）
カテゴリ：コンテンツレベルで違う場合に分ける。例：0:ゲーム 1:動画 2:独自アプリ
など

- WEARER_ID_TXT（ 装着者番号やチャンネル名の設定
装着者：同一カテゴリで複数人が使う場合に、Player 1, Player 2
など提示する振動を分けたい場合に区別
チャンネル：同一コンテンツで、再生する振動を分けたいときに利用。例：全て、銃撃のみ、足音のみ

ただし機能的には PLAY_CATEGORY と WEARER_ID は同等ですので、
送信先を分けられる変数が2つある、という認識で構いません。
*/

#ifdef NECKLACE_V2
// カテゴリ名の設定
const char *PLAY_CATEGORY_TXT[] = {"Ch 1", "Ch 2", "Ch 3"};
// 各 channel における固定モード時のボリューム値。
// 0--31まで32段階、数値は大きいほど振動が大きくなる
const int FIX_GAIN_STEP[] = {21, 16, 5};

// 装着者番号やチャンネル名の設定
const char *WEARER_ID_TXT[] = {"#1", "#2", "#3", "#4"};

// 振動強度の強さ 0--31の段階表示（描画領域を節約したい場合）
// デシベル表記は参照
const char *DECIBEL_TXT[] = {"0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
                             "8",  "9",  "10", "11", "12", "13", "14", "15",
                             "16", "17", "18", "19", "20", "21", "22", "23",
                             "24", "25", "26", "27", "28", "29", "30", "31"};

// 各項目の表示位置の調整（x, y）ディスプレイの大きさは 128×32（ピクセル単位）
const int PLAY_CATEGORY_POS[2] = {0, 8};
const int CHANNEL_ID_POS[2] = {58, 8};
const int GAIN_STEP_POS[2] = {93, 8};
const int DISP_ROT = 0;   // ディスプレイの向き（deg）上下反転にするなら90
const int FONT_SIZE = 1;  // テキストサイズが1のときy=8で縦方向の中心

// ボリュームの固定/変動モード字のLEDカラー設定（r, g, b) 8bit
const CRGB COLOR_FIX_MODE = CRGB(10, 10, 10);
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 10);
// 装着位置の指定
const int DEVICE_POS = 0;
#endif

#ifdef BAND_V2

const int DISP_ROT = 90;                  // 右利き用
const int FIX_GAIN_STEP[] = {21, 15, 0};  // 各 playCategory に対応 0--63
const char *PLAY_CATEGORY_TXT[] = {"Ch 1", "Ch 2", "Ch 3"};
const char *WEARER_ID_TXT[] = {"#1", "#2", "#3", "#4"};
const CRGB COLOR_FIX_MODE = CRGB(0, 0, 2);  // 無制限
const CRGB COLOR_VOL_MODE = CRGB(2, 0, 0);  // 制限
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);
const int BAT_NOTIFY_SOC = 5;               // 残量低下を通知する閾値（％）
const int BAT_NOTIFY_VOL = 3600;            // 残量低下を通知する閾値（mV）
const unsigned int DISPLAY_TIMEOUT = 3000;  // 3000ミリ秒
const unsigned int BATTERY_STATUS_INTERVAL = 30000;  // 30000ミリ秒
const int PLAY_CATEGORY_POS[] = {0, 8};
const int CHANNEL_ID_POS[] = {58, 8};
const int GAIN_STEP_POS[] = {93, 8};
const int FONT_SIZE = 1;  // テキストサイズが1のときy=8で縦方向の中心

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
const int DEVICE_POS = 5;  //

#endif

// デシベル（dB）での表示
// const char *DECIBEL_TXT[] = {
//     "-75",  "-34",  "-22",  "-10",  "-5",   "0",    "3.0",  "4.4",
//     "5.2",  "6.0",  "6.8",  "7.6",  "8.4",  "9.2",  "10.0", "10.8",
//     "11.6", "12.4", "13.2", "14.0", "14.8", "15.6", "16.4", "17.2",
//     "18.0", "18.8", "19.6", "20.4", "21.2", "22.0", "22.8", "24.0"};

// ==============================
// 以下は原則変更しないでください
// ==============================

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
