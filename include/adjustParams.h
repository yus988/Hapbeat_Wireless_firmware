#ifndef ADJUST_PARAMS_H
#define ADJUST_PARAMS_H

#include <FastLED.h>

// 頻繁に調整したい変数はこちらで調整
extern const int FIX_GAIN_STEP[];
extern const char *CATEGORY_ID_TXT[];
extern const char *CHANNEL_ID_TXT[];
extern const char *GAIN_STEP_TXT[];
extern const CRGB COLOR_FIX_MODE;
extern const CRGB COLOR_VOL_MODE;
extern const CRGB COLOR_DANGER_MODE;
extern const float current_thresholds[];
extern const int shutdownCycles[];
extern const int restoreCycles;
extern const int BAT_NOTIFY_SOC;
extern const int BAT_NOTIFY_VOL;
extern const int DISP_ROT;
extern const unsigned int DISPLAY_TIMEOUT;
extern const unsigned int BATTERY_STATUS_INTERVAL;

extern const int CATEGORY_TEXT_POS[2];
extern const int CHANNEL_TEXT_POS[2];
extern const int GAIN_STEP_TEXT_POS[2];
extern const int BATTERY_TEXT_POS[2];
extern const int FONT_SIZE;
extern const int DEVICE_POS;

// Wired環境用設定
extern const bool ENABLE_WIRED_MUTE_FUNCTION;

extern const int ADJ_VOL_STEP;

// 各配列のサイズを定義
// ESPNOWで使用
extern const int CATEGORY_ID_TXT_SIZE;
extern const int CHANNEL_ID_TXT_SIZE;
extern const int GAIN_STEP_TXT_SIZE;
// MQTTのみで使用
extern const int LIMITED_IDS[];
extern const char *LIMIT_ENABLE_MSG[];
struct ID_definitions {
  uint8_t blue;
  uint8_t yellow;
  uint8_t red;
  uint8_t notify;  // 電池残量減少
};
extern const int DISP_MSG_SIZE;     // DISP_MSGのサイズを保持
extern const int LIMITED_IDS_SIZE;  // LIMITED_IDSのサイズを保持
extern const ID_definitions ID_MSG;

struct MessageData {
  uint8_t id;
  const char *message;
};
extern const MessageData DISP_MSG[];

#endif
