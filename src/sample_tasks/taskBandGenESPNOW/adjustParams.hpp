// Parameters for TASK_BAND_GEN_ESPNOW (Band V2/V3)

const int FIX_GAIN_STEP[] = {16, 0,
                             23}; // 各カテゴリごとの固定ボリューム値(0--23)
// 注意: この配列の要素数を変更した場合、同じディレクトリの
// audioManagerSettings.hpp 内の CATEGORY_NUM も同じ値に更新してください。
const char *CATEGORY_ID_TXT[] = {"Cat_0"};
const char *CHANNEL_ID_TXT[] = {"CH_0", "CH_1", "CH_2", "CH_3"};
const CRGB COLOR_FIX_MODE = CRGB(5, 5, 5); //固定モード時のLEDカラー
const CRGB COLOR_VOL_MODE = CRGB(0, 0, 5); //ボリュームモード時のLEDカラー

const char *GAIN_STEP_TXT[] = {"v 0",  "v 1",  "v 2",  "v 3",  "v 4",  "v 5",  "v 6",  "v 7",
                                      "v 8",  "v 9",  "v10", "v11", "v12", "v13", "v14", "v15",
                                      "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23"};

const int ADJ_VOL_STEP = 4;
const int DISP_ROT = 90;
const int CATEGORY_TEXT_POS[2] = {0, 0};
const int CHANNEL_TEXT_POS[2] = {40, 0};
const int GAIN_STEP_TEXT_POS[2] = {84, 0};
const int BATTERY_TEXT_POS[2] = {0, 16};

const int FONT_SIZE = 1;
const int DEVICE_POS = 5;

const int CATEGORY_ID_TXT_SIZE = sizeof(CATEGORY_ID_TXT) / sizeof(CATEGORY_ID_TXT[0]);
const int CHANNEL_ID_TXT_SIZE = sizeof(CHANNEL_ID_TXT) / sizeof(CHANNEL_ID_TXT[0]);
const int GAIN_STEP_TXT_SIZE = sizeof(GAIN_STEP_TXT) / sizeof(GAIN_STEP_TXT[0]);

// adjustParams.hpp 側からは音声設定マクロを持たない（audioManagerSettings.hpp に分離）





