#pragma once

// private aggregate header to dispatch to specific private task when defined
// 新しい private タスクを作ったら、対応する TASK_* に合わせて 1 行追加してください。

#if defined(TASK_MY_PRIVATE)
  #include "MyPrivateTask/adjustParams.hpp"
#elif defined(NEW_TASK)
  #include "NewTask/adjustParams.hpp"
#endif


