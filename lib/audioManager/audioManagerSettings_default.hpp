#ifndef AUDIO_MANAGER_SETTINGS_DEFAULT_HPP
#define AUDIO_MANAGER_SETTINGS_DEFAULT_HPP

// Default (safe) settings used when a task-specific audioManagerSettings.hpp
// is not provided in the include path.

// Category count (keep small to limit RAM). Override per task if needed.
#ifndef CATEGORY_NUM
  #define CATEGORY_NUM 3
#endif

// Max number of sound files indexed from data/
#ifndef SOUND_FILE_NUM
  #define SOUND_FILE_NUM 60
#endif

// Max data entries per category
#ifndef DATA_NUM
  #define DATA_NUM 25
#endif

// Max sub variations per data entry
#ifndef SUB_DATA_NUM
  #define SUB_DATA_NUM 6
#endif

// Whether to bypass category filtering (event/demo use)
#ifndef IS_EVENT_MODE
  #define IS_EVENT_MODE false
#endif

// Max volume step (0..VOLUME_MAX)
#ifndef VOLUME_MAX
  #define VOLUME_MAX 23
#endif

// Number of concurrent playback stubs/mixers
#ifndef STUB_NUM
  #define STUB_NUM 6
#endif

// Number of device position slots
#ifndef POSITION_NUM
  #define POSITION_NUM 1
#endif

#endif

