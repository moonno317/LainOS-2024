typedef enum {
  POLICY_REALTIME,
  POLICY_INTERACTIVE,
  POLICY_NORMAL,
  POLICY_BACKGROUND } SchedulingPolicy;

typedef struct {
  int id;
  int priority;
  SchedulingPolicy policy; } Task;

Task taskList[] = {
  {1, 10, POLICY_NORMAL},
  {2, 5, POLICY_INTERACTIVE},
  {3, 15, POLICY_NORMAL},
  {4, 8, POLICY_BACKGROUND},
  {5, 12, POLICY_INTERACTIVE} };

#define NUM_TASKS (sizeof(taskList) / sizeof(Task))

Task* selectHighestPriorityTask(SchedulingPolicy policy) {
  Task* highestPriorityTask = NULL;
  int highestPriority = -1;

  for (unsigned int i = 0; i < NUM_TASKS; i++) {
    Task* task = &taskList[i];
    if (task->policy == policy && task->priority > highestPriority) {
      highestPriorityTask = task;
      highestPriority = task->priority; } } return highestPriorityTask; }

void schedule() {
  Task* highestPriorityTask = NULL;
  highestPriorityTask = selectHighestPriorityTask(POLICY_REALTIME);
  if (highestPriorityTask == NULL) {
    highestPriorityTask = selectHighestPriorityTask(POLICY_INTERACTIVE); }
  if (highestPriorityTask == NULL) {
    highestPriorityTask = selectHighestPriorityTask(POLICY_NORMAL); }
  if (highestPriorityTask == NULL) {
    highestPriorityTask = selectHighestPriorityTask(POLICY_BACKGROUND); }
if (highestPriorityTask != NULL) {
    printf("", highestPriorityTask->id); } else {
    printf("No tasks available. Performing idle operations.\n"); } }
