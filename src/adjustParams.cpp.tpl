// adjustParams.cpp template (developer-facing)
// 目的: 新しい環境/タスクで UI/表示まわりのパラメータを定義するための雛形
// 使い方: 本ファイルを src/adjustParams.cpp として配置し、値を用途に応じて編集してください。
// 注意: audioManager が参照する音声処理用のマクロは "audioManagerSettings.hpp" 側で定義してください。

#include "adjustParams.h"
#include <FastLED.h>

// --- 基本（カテゴリ/チャンネル/固定ゲイン） ---
// カテゴリ名（日本語可）
const char *CATEGORY_ID_TXT[] = {"cat_0", "cat_1", "cat_2"};
// チャンネル/装着者などの識別名
const char *CHANNEL_ID_TXT[] = {"ch_0", "ch_1", "ch_2", "ch_3"};
// 各カテゴリにおける固定モード時のゲイン（0..23）
const int FIX_GAIN_STEP[] = {16, 16, 16};

// 表示用のゲイン段階（0..23）
const char *GAIN_STEP_TXT[] = {
  "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
  "8",  "9",  "10", "11", "12", "13", "14", "15",
  "16", "17", "18", "19", "20", "21", "22", "23"
};

// --- UI 表示レイアウト ---
// 128x32 OLED を前提に、各テキストの表示座標を指定
const int CATEGORY_TEXT_POS[2] = {0, 8};
const int CHANNEL_TEXT_POS[2]  = {54, 8};
const int GAIN_STEP_TEXT_POS[2]= {100, 8};
// バッテリー表示を行う場合の原点
const int BATTERY_TEXT_POS[2]  = {0, 16};

// ディスプレイ設定
const int DISP_ROT = 0;   // 0 or 90（右利き向けは 90 を推奨）
const int FONT_SIZE = 1;  // Adafruit_GFX のテキストサイズ

// LED 表示色
const CRGB COLOR_FIX_MODE = CRGB(5, 5, 5);  // 固定モード
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 5);  // 可変モード
// 危険表示色（必要に応じて使用）
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);

// ボリューム調整のステップ幅（可変モード時）
const int ADJ_VOL_STEP = 4;

// 有線モード向け設定（未使用ならそのままでOK）
const bool ENABLE_WIRED_MUTE_FUNCTION = false;
// 装着位置（アプリ側運用値。例: 0=Neck/5=Arm など）
const int DEVICE_POS = 0;

// --- サイズ定義（自動計算） ---
const int CATEGORY_ID_TXT_SIZE = sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
const int CHANNEL_ID_TXT_SIZE  = sizeof(CHANNEL_ID_TXT)  / sizeof(CHANNEL_ID_TXT[0]);
const int GAIN_STEP_TXT_SIZE   = sizeof(GAIN_STEP_TXT)   / sizeof(GAIN_STEP_TXT[0]);

// --- バッテリー/システム関連（必要に応じて有効化） ---
// const unsigned int BATTERY_STATUS_INTERVAL = 30000; // ms 単位
// const int BAT_NOTIFY_SOC = 5;    // % 残量閾値
// const int BAT_NOTIFY_VOL = 3600; // mV 閾値
// const unsigned int DISPLAY_TIMEOUT = 3000; // ms 無操作で display OFF

// --- MQTT 関連（必要な場合のみ使用） ---
// const ID_definitions ID_MSG = {0, 1, 2, 3};
// const MessageData DISP_MSG[] = {
//   {0, "Blue Lamp"}, {1, "Yellow Lamp"}, {2, "Red Lamp"}, {3, "Msg 4"}
// };
// const int LIMITED_IDS[] = {0, 1};
// const char *LIMIT_ENABLE_MSG[] = {"All", "Limited"};
// const int LIMITED_IDS_SIZE = sizeof(LIMITED_IDS) / sizeof(LIMITED_IDS[0]);
// const int DISP_MSG_SIZE = sizeof(DISP_MSG) / sizeof(DISP_MSG[0]);
