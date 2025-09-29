#pragma once

// private aggregate header to dispatch to specific private task when defined

#if defined(TASK_NECK_DEMO_ESPNOW)
  #include "private_tasks/taskNeckDemoESPNOW/adjustParams.hpp"
#elif defined(TASK_JUDO0806)
  #include "private_tasks/taskJUDO0806/adjustParams.hpp"
#endif


