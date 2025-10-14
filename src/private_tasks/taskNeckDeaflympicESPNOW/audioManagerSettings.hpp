#ifndef AUDIO_MANAGER_SETTINGS_HPP
#define AUDIO_MANAGER_SETTINGS_HPP
// Audio categories count.
// 注意: `adjustParams.hpp` の CATEGORY_ID_TXT の要素数を変更した場合、
//       ここも同じ値に更新してください。
#define CATEGORY_NUM 1

// Max number of sound files to index and load metadata for (data/ files)
#define SOUND_FILE_NUM 50

// Max data entries per category (sound_id range per category)
#define DATA_NUM 6

// Max sub variations per data (sub_id range)
#define SUB_DATA_NUM 7

// Event mode: if true, ignore category filter and allow all
#define IS_EVENT_MODE false

// Max volume step for amp (0..VOLUME_MAX)
#define VOLUME_MAX 10

// Max concurrent stubs (mixers) (e.g., need 6 for demo use cases)
#define STUB_NUM 6

// Device position slots (keep 1 unless multi-position addressing is required)
#define POSITION_NUM 1

// ループ継続のkeep-aliveタイムアウト（ミリ秒）
// ホストからの continue(=9) 未受信がこの時間続くとループを停止する
#define LOOP_CONTINUE_TIMEOUT_MS 3000

#endif
