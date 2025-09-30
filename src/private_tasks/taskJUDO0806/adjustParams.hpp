// Parameters for TASK_JUDO0806 (private)
#pragma once

#include <FastLED.h>

// 強い定義で弱シンボルを上書きし、UIの段数や表示を有効化する
const char *CATEGORY_ID_TXT[] = {"Cat0", "Cat1", "Cat2"};
const char *CHANNEL_ID_TXT[]  = {"CH0", "CH1", "CH2", "CH3"};
const int  FIX_GAIN_STEP[]    = {16, 16, 16};

const CRGB COLOR_FIX_MODE = CRGB(5, 5, 5);
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 5);

// 0..23 の24段（ADC→ゲイン段マップと整合）
const char *GAIN_STEP_TXT[] = {
  "0","1","2","3","4","5","6","7","8","9",
  "10","11","12","13","14","15","16","17","18","19","20","21","22","23"
};

// UI/動作パラメータ
const int ADJ_VOL_STEP = 4;
const int DISP_ROT     = 90;   // right-handed
const int DEVICE_POS   = 0;

// 位置・サイズ（必要なら調整）
const int CATEGORY_TEXT_POS[2]  = {0, 8};
const int CHANNEL_TEXT_POS[2]   = {54, 8};
const int GAIN_STEP_TEXT_POS[2] = {100, 8};
const int FONT_SIZE             = 1;

// サイズ定義
const int CATEGORY_ID_TXT_SIZE = sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
const int CHANNEL_ID_TXT_SIZE  = sizeof(CHANNEL_ID_TXT) / sizeof(CHANNEL_ID_TXT[0]);
const int GAIN_STEP_TXT_SIZE   = sizeof(GAIN_STEP_TXT) / sizeof(GAIN_STEP_TXT[0]);
