#pragma once

#include <Core/types.h>

#include <vector>
#include <functional>
#include <mutex>

#define assert_called_by_main_thread __AssetCalledByMainThread

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

void InitTaskManager();
void InitTaskManager (TaskManager* manager);

void TerminateTaskManager();
void TerminateTaskManager (TaskManager* manager);

// DONT SUBMIT NEW WORK IN A FINALIZER!
void SubmitTask(std::function<void(U32 workerID)> execute, std::function<void(U32 workerID)> finalize);

TaskManager* __InternalGetTaskManager();
void __AssetCalledByMainThread();

template <typename TTask, typename... TArgs>
void ScheduleTask(TArgs... args) {
	static_assert(std::is_base_of<ITask, TTask>::value, "Scheduled Tasks Must Inherit from the ITask Interface!");
	WorkQueue* queue = &__InternalGetTaskManager()->workQueue;

	TTask* task = new TTask(args...);
	queue->mutex.lock();
	queue->tasksToExecute.push_back(task);
	queue->condition_variable.notify_one();
	queue->mutex.unlock();
}

void FinializeCompletedTasks();