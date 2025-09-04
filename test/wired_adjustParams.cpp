// 頻繁に調整したい変数はこちらで調整
#include "adjustParams.h"

/* カテゴリと装着者やチャンネルの意図について

- CATEGORY_ID_TXT
カテゴリ：コンテンツレベルで違う場合に分ける。例：0:ゲーム 1:動画 2:独自アプリ
など

- CHANNEL_ID_TXT 装着者番号やチャンネル名の設定
装着者：同一カテゴリで複数人が使う場合に、Player 1, Player 2
など提示する振動を分けたい場合に区別
チャンネル：同一コンテンツで、再生する振動を分けたいときに利用。例：全て、銃撃のみ、足音のみ

ただし機能的には CATEGORY_ID_TXT と CHANNEL_ID_TXT は同等ですので、
送信先を分けられる変数が2つある、という認識で構いません。
*/

#ifdef NECKLACE_V2
// カテゴリ名の設定（日本語OK）
// ※カテゴリの数を4以上にする場合は、./lib/audioManager/audioManager.h の
// CATEGORY_NUM の値を変更してください。デフォルトでは3となっています。
const char *CATEGORY_ID_TXT[] = {"cat_0", "cat_1", "cat_2"};
// 装着者番号やチャンネル名の設定
const char *CHANNEL_ID_TXT[] = {"ch_0", "ch_1", "ch_2", "ch_3"};

  // 各 カテゴリにおける固定モード時のボリューム値。
// 0--23まで24段階、数値は大きいほど振動が大きくなる
const int FIX_GAIN_STEP[] = {20, 16, 5};

// 振動強度の強さ 0--23の段階表示
const char *GAIN_STEP_TXT[] = {"0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
                               "8",  "9",  "10", "11", "12", "13", "14", "15",
                               "16", "17", "18", "19", "20", "21", "22", "23"};

// 各項目の表示位置の調整（x, y）ディスプレイの大きさは 128×32（ピクセル単位）
const int CATEGORY_TEXT_POS[2] = {0, 8};
const int CHANNEL_TEXT_POS[2] = {54, 8};
const int GAIN_STEP_TEXT_POS[2] = {100, 8};

#ifdef WIRED
const int DISP_ROT = 90;   // ディスプレイの向き（deg）上下反転にするなら90
const int FONT_SIZE = 1;  // テキストサイズが1のときy=8で縦方向の中心
// ボリュームの固定/変動モード字のLEDカラー設定（r, g, b) 8bit
const CRGB COLOR_FIX_MODE = CRGB(0, 0, 10);
const CRGB COLOR_VOL_MODE = CRGB(10, 10, 10);
#else       
const int DISP_ROT = 0;   // ディスプレイの向き（deg）上下反転にするなら90
const int FONT_SIZE = 8;  // テキストサイズが1のときy=8で縦方向の中心
// ボリュームの固定/変動モード字のLEDカラー設定（r, g, b) 8bit
const CRGB COLOR_FIX_MODE = CRGB(10, 10, 10);
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 10);
#endif

// 装着位置の指定
const int DEVICE_POS = 0;  // 首
#endif

#ifdef BAND_V2
// 各 カテゴリにおける固定モード時のボリューム値。
// 0--23まで24段階、数値は大きいほど振動が大きくなる
const int FIX_GAIN_STEP[] = {16, 0, 23};
// ※カテゴリの数を4以上にする場合は、./lib/audioManager/audioManager.h の
// CATEGORY_NUM の値を変更してください。デフォルトでは3となっています。
const char *CATEGORY_ID_TXT[] = {"cat_0", "cat_1", "cat_2"};
// 装着者番号やチャンネル名の設定
const char *CHANNEL_ID_TXT[] = {"ch_0", "ch_1", "ch_2", "ch_3"};
const CRGB COLOR_FIX_MODE = CRGB(5, 5, 5);
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 5);
// 振動強度の強さ 0--23の段階表示
const char *GAIN_STEP_TXT[] = {"0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
                               "8",  "9",  "10", "11", "12", "13", "14", "15",
                               "16", "17", "18", "19", "20", "21", "22", "23"};
const int ADJ_VOL_STEP = 4;  // 音量調整モードでのステップ量
const int DISP_ROT = 90;     // 右利き用
const int CATEGORY_TEXT_POS[2] = {0, 8};
const int CHANNEL_TEXT_POS[2] = {54, 8};
const int GAIN_STEP_TEXT_POS[2] = {100, 8};
const int FONT_SIZE = 1;  // テキストサイズが1のときy=8で縦方向の中心
// 装着位置の指定
const int DEVICE_POS = 5;  // 左腕

  #ifdef MQTT
const unsigned int BATTERY_STATUS_INTERVAL = 30000;
const CRGB COLOR_FIX_MODE = CRGB(0, 0, 2);  // 無制限
const CRGB COLOR_VOL_MODE = CRGB(2, 0, 0);  // 制限
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);
const int BAT_NOTIFY_SOC = 5;                // 残量低下を通知する閾値（％）
const int BAT_NOTIFY_VOL = 3600;             // 残量低下を通知する閾値（mV）
const unsigned int DISPLAY_TIMEOUT = 3000;   // 3000ミリ秒
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

// ==============================
// 以下は原則変更しないでください
// ==============================

// 各配列のサイズを定義
// ESPNOW, WIRED, MQTT 共通で使用
const int CATEGORY_ID_TXT_SIZE =
    sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
const int CHANNEL_ID_TXT_SIZE =
    sizeof(CHANNEL_ID_TXT) / sizeof(CHANNEL_ID_TXT[0]);
const int GAIN_STEP_TXT_SIZE = sizeof(GAIN_STEP_TXT) / sizeof(GAIN_STEP_TXT[0]);

// MQTTで追加使用
#ifdef MQTT
const int LIMITED_IDS_SIZE =
    sizeof(LIMITED_IDS) / sizeof(LIMITED_IDS[0]);                  // サイズ定義
const int DISP_MSG_SIZE = sizeof(DISP_MSG) / sizeof(DISP_MSG[0]);  // サイズ定義
#endif
