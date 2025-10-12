#pragma once
// Deaflympic task UI専用定義（taskNeckGenESPNOW.cpp 内でのみ使用）

// ボタンインデックスは共通マクロ（include/adjustParams.h）を参照します
// このタスクでは上下反転に伴い、ロジカルなボタン位置を再マッピングします
const int LOGICAL_BTN_IDX_TOP_LEFT = DUOWL_BTN_IDX_BOTTOM_RIGHT;
const int LOGICAL_BTN_IDX_TOP_RIGHT = DUOWL_BTN_IDX_BOTTOM_LEFT;
const int LOGICAL_BTN_IDX_CENTER = DUOWL_BTN_IDX_CENTER;
const int LOGICAL_BTN_IDX_BOTTOM_LEFT = DUOWL_BTN_IDX_TOP_RIGHT;
const int LOGICAL_BTN_IDX_BOTTOM_RIGHT = DUOWL_BTN_IDX_TOP_LEFT;

// モードごとのIDセット（端末側フィルタに使用）
const int MODE_PLAY_ALL_IDS[] = {0, 1, 2, 3, 4, 5};
const int MODE_PLAY_ALL_IDS_SIZE = 6;
const int MODE_NO_SCRAPE_IDS[] = {1, 2, 3, 4, 5};
const int MODE_NO_SCRAPE_IDS_SIZE = 5;
const int MODE_VENUE_ONLY_IDS[] = {3, 4, 5};
const int MODE_VENUE_ONLY_IDS_SIZE = 3;
const int MODE_ARENA_ONLY_IDS[] = {0, 1, 2};
const int MODE_ARENA_ONLY_IDS_SIZE = 3;

// テストモードの無操作戻り時間（ミリ秒）
const unsigned long MODE_TEST_IDLE_TIMEOUT_MS = 60000UL;

// 表示ラベル（メニュー用：上段=JA, 下段=EN）
const char *MENU_PLAYALL_JA = "全て再生";
const char *MENU_PLAYALL_EN = "Play all";
const char *MENU_NOSCRAPE_JA = "すり足 無し";
const char *MENU_NOSCRAPE_EN = "No Scrape";
const char *MENU_VENUE_JA = "会場の音のみ";
const char *MENU_VENUE_EN = "Venue sound only";
const char *MENU_ARENA_JA = "競技所の音のみ";
const char *MENU_ARENA_EN = "Arena sound only";

// テストモード用ラベル（id=0..5）
const char *TEST_ID_LABEL_JA[6] = {"すり足", "軽打", "重打", "拍手", "歓声", "ブザー"};
const char *TEST_ID_LABEL_EN[6] = {"Slide", "Tap", "Hit", "Clap", "Cheer", "Beep"};

// ボリュームUI設定（@taskJUDO0806 と同一）
const int VOL_UI_BAR_X = 10;
const int VOL_UI_BAR_Y = 10;  // @taskJUDO0806 と同じ位置
const int VOL_UI_BAR_W = 80;  // @taskJUDO0806 と同じ幅
const int VOL_UI_BAR_H = 12;
const int VOL_UI_TEXT_SIZE = 2;

// ちらつき防止：この値以上の変化があった時のみ画面更新
// 例: 1なら毎回更新、2なら2段階変化で更新、3以上推奨
const int VOL_UI_CHANGE_THRESHOLD = 2;  // ±20前後の窓枠（調整可能）

// 無操作でUIを消してメニューへ戻る時間
const unsigned long VOL_UI_TIMEOUT_MS = 1000UL;

