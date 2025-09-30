#pragma once

#include <FastLED.h>

inline const char *CATEGORY_ID_TXT[] = {"MyCat"};
inline const char *CHANNEL_ID_TXT[]  = {"CH_0"};
inline const char *GAIN_STEP_TXT[]   = {"v0","v1","v2","v3","v4","v5","v6","v7","v8","v9","v10","v11","v12","v13","v14","v15","v16","v17","v18","v19","v20","v21","v22","v23"};

inline const int CATEGORY_TEXT_POS[2]  = {0, 0};
inline const int CHANNEL_TEXT_POS[2]   = {40, 0};
inline const int GAIN_STEP_TEXT_POS[2] = {84, 0};
inline const int BATTERY_TEXT_POS[2]   = {0, 16};
inline const int FONT_SIZE             = 1;
inline const int DISP_ROT              = 90;
inline const CRGB COLOR_FIX_MODE       = CRGB(5, 5, 5);
inline const CRGB COLOR_VOL_MODE       = CRGB(0, 0, 5);
inline const CRGB COLOR_DANGER_MODE    = CRGB(10, 0, 0);

inline const int FIX_GAIN_STEP[] = {16};
inline const int ADJ_VOL_STEP    = 4;
inline const int DEVICE_POS      = 5;

inline const int CATEGORY_ID_TXT_SIZE = sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
inline const int CHANNEL_ID_TXT_SIZE  = sizeof(CHANNEL_ID_TXT) / sizeof(CHANNEL_ID_TXT[0]);
inline const int GAIN_STEP_TXT_SIZE   = sizeof(GAIN_STEP_TXT) / sizeof(GAIN_STEP_TXT[0]);


