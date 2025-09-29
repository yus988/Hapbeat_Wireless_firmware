// 頻繁に調整したい変数はこちらで調整（環境別ファイルを取り込む集約ファイル）
#include "adjustParams.h"

// 注意:
// - lib/ 以下から参照される可能性があるため、extern 宣言は include/adjustParams.h に集約
// - 実体定義はビルド環境ごとにここで取り込む .inc 側に寄せる

// TASK_* に応じて一意の .hpp を取り込む
#if defined(TASK_BAND_GEN_ESPNOW)
  #include "public_tasks/taskBandGenESPNOW/adjustParams.hpp"
#elif defined(TASK_NECK_GEN_ESPNOW)
  #include "public_tasks/taskNeckGenESPNOW/adjustParams.hpp"
#elif defined(TASK_NECK_DEMO_ESPNOW)
  #include "private_tasks/taskNeckDemoESPNOW/adjustParams.hpp"
#elif defined(TASK_BAND_GEN_MQTT)
  #include "public_tasks/taskBandGenMQTT/adjustParams.hpp"
#elif defined(TASK_NECK_GEN_WIRED)
  #include "public_tasks/taskNeckGenWIRED/adjustParams.hpp"
#elif defined(TASK_JUDO0806)
  // JUDO0806 は UI 実体を個別に持たず、弱シンボルのデフォルトを使用します
#else
  #error "Define one of TASK_NECK_GEN_ESPNOW, TASK_BAND_GEN_ESPNOW, TASK_BAND_GEN_MQTT, TASK_NECK_GEN_WIRED, TASK_JUDO0806"
#endif

// サイズ定義は各 adjustProfiles/*.hpp 内で inline 変数として提供
