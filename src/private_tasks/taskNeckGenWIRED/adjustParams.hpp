// Parameters for TASK_NECK_GEN_WIRED (Necklace)

// 注意: この配列の要素数を変更した場合、同じディレクトリの
// audioManagerSettings.hpp 内の CATEGORY_NUM も同じ値に更新してください。
const char *CATEGORY_ID_TXT[] = {"cat_0", "cat_1", "cat_2"};
const char *CHANNEL_ID_TXT[] = {"ch_0", "ch_1", "ch_2", "ch_3"};
const int FIX_GAIN_STEP[] = {12, 16, 5};
#ifdef BAND_V3
const char *GAIN_STEP_TXT[] = {"v0",  "v1",  "v2",  "v3",  "v4",  "v5",  "v6",  "v7",
                                      "v8",  "v9",  "v10", "v11", "v12", "v13", "v14", "v15",
                                      "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"};
#else
const char *GAIN_STEP_TXT[] = {"0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
                                      "8",  "9",  "10", "11", "12", "13", "14", "15",
                                      "16", "17", "18", "19", "20", "21", "22", "23"};
#endif
const int CATEGORY_TEXT_POS[2] = {0, 0};
const int CHANNEL_TEXT_POS[2] = {54, 0};
const int GAIN_STEP_TEXT_POS[2] = {100, 0};
const int DISP_ROT = 90;  // right-handed
const int FONT_SIZE = 1;
const CRGB COLOR_FIX_MODE = CRGB(0, 0, 10);
const CRGB COLOR_VOL_MODE = CRGB(10, 10, 10);
const bool ENABLE_WIRED_MUTE_FUNCTION = false;
const int DEVICE_POS = 0;  // neck

const int CATEGORY_ID_TXT_SIZE = sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
const int CHANNEL_ID_TXT_SIZE = sizeof(CHANNEL_ID_TXT) / sizeof(CHANNEL_ID_TXT[0]);
const int GAIN_STEP_TXT_SIZE = sizeof(GAIN_STEP_TXT) / sizeof(GAIN_STEP_TXT[0]);
// 音声設定マクロは audioManagerSettings.hpp に分離