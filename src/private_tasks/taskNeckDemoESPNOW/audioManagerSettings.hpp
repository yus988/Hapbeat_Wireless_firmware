#ifndef AUDIO_MANAGER_SETTINGS_NECK_DEMO_ESPNOW_HPP
#define AUDIO_MANAGER_SETTINGS_NECK_DEMO_ESPNOW_HPP

// Audio categories count.
// 豕ｨ諢・ `adjustParams.hpp` 縺ｮ CATEGORY_ID_TXT 縺ｮ隕∫ｴ謨ｰ繧貞､画峩縺励◆蝣ｴ蜷医・//       縺薙％繧ょ酔縺伜､縺ｫ譖ｴ譁ｰ縺励※縺上□縺輔＞縲・#define CATEGORY_NUM 3

// Max number of sound files to index and load metadata for (data/ files)
#define SOUND_FILE_NUM 60

// Max data entries per category (sound_id range per category)
#define DATA_NUM 25

// Max sub variations per data (sub_id range)
#define SUB_DATA_NUM 6

// Event mode: if true, ignore category filter and allow all
#define IS_EVENT_MODE false

// Max volume step for amp (0..VOLUME_MAX)
#define VOLUME_MAX 23

// Max concurrent stubs (mixers) (e.g., need 6 for demo use cases)
#define STUB_NUM 6

// Device position slots (keep 1 unless multi-position addressing is required)
#define POSITION_NUM 1

#endif
