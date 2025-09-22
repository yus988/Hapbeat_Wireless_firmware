// Parameters for TASK_BAND_GEN_MQTT (Band)

const int FIX_GAIN_STEP[] = {16, 0, 23};
#ifdef BAND_V3
// 注意: この配列の要素数を変更した場合、同じディレクトリの
// audioManagerSettings.hpp 内の CATEGORY_NUM も同じ値に更新してください。
const char *CATEGORY_ID_TXT[] = {"C0", "C1", "C2"};
const char *CHANNEL_ID_TXT[] = {"CH0", "CH1", "CH2", "CH3"};
#else
// 注意: この配列の要素数を変更した場合、同じディレクトリの
// audioManagerSettings.hpp 内の CATEGORY_NUM も同じ値に更新してください。
const char *CATEGORY_ID_TXT[] = {"cat_0", "cat_1", "cat_2"};
const char *CHANNEL_ID_TXT[] = {"ch_0", "ch_1", "ch_2", "ch_3"};
#endif
const CRGB COLOR_FIX_MODE = CRGB(0, 0, 2);
const CRGB COLOR_VOL_MODE = CRGB(2, 0, 0);
const CRGB COLOR_DANGER_MODE = CRGB(10, 0, 0);
#ifdef BAND_V3
const char *GAIN_STEP_TXT[] = {"v0",  "v1",  "v2",  "v3",  "v4",  "v5",  "v6",  "v7",
                                      "v8",  "v9",  "v10", "v11", "v12", "v13", "v14", "v15",
                                      "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"};
#else
const char *GAIN_STEP_TXT[] = {"0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
                                      "8",  "9",  "10", "11", "12", "13", "14", "15",
                                      "16", "17", "18", "19", "20", "21", "22", "23"};
#endif
const int ADJ_VOL_STEP = 4;
const int DISP_ROT = 90;
const int CATEGORY_TEXT_POS[2] = {0, 0};
const int CHANNEL_TEXT_POS[2] = {40, 0};
const int GAIN_STEP_TEXT_POS[2] = {84, 0};
#ifdef BAND_V3
const int BATTERY_TEXT_POS[2] = {0, 16};
#else
const int BATTERY_TEXT_POS[2] = {96, 16};
#endif
const int FONT_SIZE = 1;
const int DEVICE_POS = 5;

// MQTT specific
const unsigned int BATTERY_STATUS_INTERVAL = 30000;
const int BAT_NOTIFY_SOC = 5;
const int BAT_NOTIFY_VOL = 3600;
const unsigned int DISPLAY_TIMEOUT = 3000;
const ID_definitions ID_MSG = {0, 1, 2, 3};
const MessageData DISP_MSG[] = {
    {0, "青 / Blue \nランプ点灯"},
    {1, "黄 / Yellow \nランプ点灯"},
    {2, "赤 / Red \nランプ点灯"},
    {3, "メッセージ 4"},
};
const int LIMITED_IDS[] = {0, 1};
const char *LIMIT_ENABLE_MSG[] = {"全て再生", "制限モード"};

const int CATEGORY_ID_TXT_SIZE = sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
const int CHANNEL_ID_TXT_SIZE = sizeof(CHANNEL_ID_TXT) / sizeof(CHANNEL_ID_TXT[0]);
const int GAIN_STEP_TXT_SIZE = sizeof(GAIN_STEP_TXT) / sizeof(GAIN_STEP_TXT[0]);
const int LIMITED_IDS_SIZE = sizeof(LIMITED_IDS) / sizeof(LIMITED_IDS[0]);
const int DISP_MSG_SIZE = sizeof(DISP_MSG) / sizeof(DISP_MSG[0]);
// 音声設定マクロは audioManagerSettings.hpp に分離




