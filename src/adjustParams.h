#ifndef ADJUST_PARAMS_H
#define ADJUST_PARAMS_H

#include <FastLED.h>

// 頻繁に調整したい変数はこちらで調整
extern const int FIX_GAIN_STEP[];
extern const char *PLAY_CATEGORY_TXT[];
extern const char *WEARER_ID_TXT[];
extern const char *DECIBEL_TXT[];
extern const int volumeThreshold;
extern const CRGB COLOR_FIX_MODE;
extern const CRGB COLOR_VOL_MODE;
extern const CRGB COLOR_DANGER_MODE;
extern const float current_thresholds[];
extern const int shutdownCycles[];
extern const int restoreCycles;
extern const int BAT_NOTIFY_SOC;
extern const int BAT_NOTIFY_VOL;
extern const int DISP_ROT;
extern const int LIMITED_IDS[];
extern const char *LIMIT_ENABLE_MSG[];

// 各配列のサイズを定義
extern const int PLAY_CATEGORY_TXT_SIZE;
extern const int WEARER_ID_TXT_SIZE;
extern const int DECIBEL_TXT_SIZE;

struct ID_definitions {
    uint8_t blue;
    uint8_t yellow;
    uint8_t red;
    uint8_t notify;  // 電池残量減少
};

extern const ID_definitions ID_MSG;

struct MessageData {
    uint8_t id;
    const char *message;
};

extern const MessageData DISP_MSG[];

#endif
