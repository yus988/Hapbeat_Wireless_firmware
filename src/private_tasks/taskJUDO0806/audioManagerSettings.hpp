#ifndef AUDIO_MANAGER_SETTINGS_HPP
#define AUDIO_MANAGER_SETTINGS_HPP

// Audio categories count. 
#define CATEGORY_NUM 3

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
