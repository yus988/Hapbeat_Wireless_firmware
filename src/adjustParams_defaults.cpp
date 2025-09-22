#include <Arduino.h>
#include <FastLED.h>
#include "adjustParams.h"

// Weak defaults for adjustParams externs.
// These allow building tasks that don't provide UI-specific adjustParams.hpp.

// Common text/labels
extern __attribute__((weak)) const char *CATEGORY_ID_TXT[] = {"cat_0"};
extern __attribute__((weak)) const char *CHANNEL_ID_TXT[]  = {"ch_0"};
extern __attribute__((weak)) const char *GAIN_STEP_TXT[]   = {"0"};

// Sizes derived from above placeholders
extern __attribute__((weak)) const int CATEGORY_ID_TXT_SIZE = sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
extern __attribute__((weak)) const int CHANNEL_ID_TXT_SIZE  = sizeof(CHANNEL_ID_TXT)  / sizeof(CHANNEL_ID_TXT[0]);
extern __attribute__((weak)) const int GAIN_STEP_TXT_SIZE   = sizeof(GAIN_STEP_TXT)   / sizeof(GAIN_STEP_TXT[0]);

// Fixed gain steps (single-category fallback)
extern __attribute__((weak)) const int FIX_GAIN_STEP[] = {16};

// Colors
extern __attribute__((weak)) const CRGB COLOR_FIX_MODE = CRGB(0, 0, 0);
extern __attribute__((weak)) const CRGB COLOR_VOL_MODE = CRGB(0, 0, 0);
extern __attribute__((weak)) const CRGB COLOR_DANGER_MODE = CRGB(0, 0, 0);

// Layout / UI params
extern __attribute__((weak)) const int CATEGORY_TEXT_POS[2] = {0, 0};
extern __attribute__((weak)) const int CHANNEL_TEXT_POS[2]  = {0, 0};
extern __attribute__((weak)) const int GAIN_STEP_TEXT_POS[2]= {0, 0};
extern __attribute__((weak)) const int BATTERY_TEXT_POS[2]  = {0, 0};
extern __attribute__((weak)) const int FONT_SIZE = 1;
extern __attribute__((weak)) const int DEVICE_POS = 0;
extern __attribute__((weak)) const int DISP_ROT = 0;
extern __attribute__((weak)) const int ADJ_VOL_STEP = 1;

// Battery / system
extern __attribute__((weak)) const int BAT_NOTIFY_SOC = 0;
extern __attribute__((weak)) const int BAT_NOTIFY_VOL = 0;
extern __attribute__((weak)) const unsigned int DISPLAY_TIMEOUT = 0;
extern __attribute__((weak)) const unsigned int BATTERY_STATUS_INTERVAL = 0;

// Wired feature flags
extern __attribute__((weak)) const bool ENABLE_WIRED_MUTE_FUNCTION = false;

// Current thresholds (fallback single entry)
extern __attribute__((weak)) const float current_thresholds[] = {0.0f};
extern __attribute__((weak)) const int shutdownCycles[] = {0};
extern __attribute__((weak)) const int restoreCycles = 0;

// MQTT related (safe zero-sized defaults)
extern __attribute__((weak)) const int LIMITED_IDS[] = {0};
extern __attribute__((weak)) const char *LIMIT_ENABLE_MSG[] = {nullptr, nullptr};
extern __attribute__((weak)) const int LIMITED_IDS_SIZE = 0;

extern __attribute__((weak)) const int DISP_MSG_SIZE = 0;
extern __attribute__((weak)) const ID_definitions ID_MSG = {0, 0, 0, 0};
extern __attribute__((weak)) const MessageData DISP_MSG[] = {{0, ""}};


