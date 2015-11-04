#pragma once

#include <vector>
#include <functional>
#include <mutex>

#include <Core/types.h>
#include <Core/logging.h>
#include <Core/config.h>

#define assert_called_by_main_thread() __assertCalledByMainThread()

struct ITask {
	virtual void execute(U32 workerID) = 0;
	virtual void finalize(U32 workerID) = 0;
	int executorWorkerID = -1;
};

struct Work {
	std::function<void(U32 workerID)> execute;
	std::function<void(U32 workerID)> finalize;
	S32 workerWhoCompletedMe = -1;
};

struct WorkQueue {
	std::mutex mutex;
	std::condition_variable condition_variable;

	std::vector<ITask*> tasksToExecute;
	std::vector<ITask*> tasksToFinialize;
};

struct TaskManager {
	std::vector<std::thread> threads;
	WorkQueue workQueue;
};

U32 GetTaskWorkerCount();
void __assertCalledByMainThread();
void __AssetCalledByMainThread();

void InitTaskManager (TaskManager* manager);
void TerminateTaskManager (TaskManager* manager);
void FinializeCompletedTasks (WorkQueue* queue);


#ifdef FORCE_SINGLE_THREADED
template <typename TTask, typename... TArgs>
void ScheduleTask(WorkQueue* queue, TArgs... args) {
	static_assert(std::is_base_of<ITask, TTask>::value, "Scheduled Tasks Must Inherit from the ITask Interface!");
	assert_called_by_main_thread();

	TTask task(args...);
	task.execute(0);
	task.finalize(0);
}

#else

template <typename TTask, typename... TArgs>
void ScheduleTask (WorkQueue* queue, TArgs... args) {
	static_assert(std::is_base_of<ITask, TTask>::value, "Scheduled Tasks Must Inherit from the ITask Interface!");
	assert_called_by_main_thread();

	TTask* task = new TTask(args...);
	std::lock_guard<std::mutex> lock (queue->mutex);
	LOG_VERBOSE("TaskManager schedueling new task...");
	queue->tasksToExecute.emplace_back(task);
	queue->condition_variable.notify_one();
}
#endif

