#ifndef TASK_ENTRY_H
#define TASK_ENTRY_H

void TaskAppInit();
void TaskAppStart();
void TaskAppLoop();
// 各タスクが提供する統一タスクエントリ（UIループ）
void TaskUI_Run(void *args);

#endif  // TASK_ENTRY_H




