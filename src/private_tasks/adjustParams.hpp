#pragma once

// private override dispatcher
// 指定の TASK_* が定義されている場合に、各 private タスク配下の adjustParams を取り込みます。

#if defined(TASK_NECK_DEMO_ESPNOW)
  #include "taskNeckDemoESPNOW/adjustParams.hpp"
#elif defined(TASK_JUDO0806)
  #include "taskJUDO0806/adjustParams.hpp"
#elif defined(TASK_BAND_GEN_MQTT)
  #include "taskBandGenMQTT/adjustParams.hpp"
#elif defined(TASK_NECK_GEN_WIRED)
  #include "taskNeckGenWIRED/adjustParams.hpp"
#elif defined(TASK_MY_PRIVATE)
  #include "MyPrivateTask/adjustParams.hpp"
#else
  // 必要に応じてここに他の private タスク分岐を追加してください。
#endif
