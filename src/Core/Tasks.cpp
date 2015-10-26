#include "Tasks.hpp"
#include <type_traits>

#include <Core/logging.h>

global_variable TaskManager g_TaskManager;
global_variable std::thread::id g_MainThreadID;

struct LambdaTask : public ITask {
	std::function<void()> execute_lambda;
	std::function<void()> finalize_lambda;
	virtual void execute(U32 workerID) final;
	virtual void finalize() final;
	LambdaTask(const std::function<void()>& execute, const std::function<void()>& finalize);
};

LambdaTask::LambdaTask(const std::function<void()>& execute, const std::function<void()>& finalize) : 
	execute_lambda(execute), finalize_lambda(finalize) {}

void LambdaTask::execute(U32 workerID) { execute_lambda(); }
void LambdaTask::finalize() { finalize_lambda(); }

static void ThreadProc(WorkQueue* queue, U32 workerID) {
	bool isRunning = true;

	ITask* task;
	bool taskNeedsExecution = false;

	while (isRunning) {
		if (taskNeedsExecution) {
			task->execute(workerID);
			task->executorWorkerID = workerID;
			taskNeedsExecution = false;

			queue->mutex.lock();
			queue->tasksToFinialize.push_back(task);
			queue->mutex.unlock();
		}


		std::unique_lock<std::mutex> lock(queue->mutex);
		queue->condition_variable.wait(lock);

		if (queue->tasksToExecute.size() > 0) {
			task = queue->tasksToExecute.back();
			queue->tasksToExecute.pop_back();
			taskNeedsExecution = true;
		}
		else {
			isRunning = false;
			LOG_INFO("Thread is exiting");
		}
	}
}

void InitTaskManager(TaskManager* manager) {
	auto threadCount = GetTaskWorkerCount();
	manager->threads.resize(threadCount);

	for (U32 i = 0; i < threadCount; i++) {
		manager->threads[i] = std::thread(ThreadProc, &manager->workQueue, i);
	}
}

void InitTaskManager() {
	g_MainThreadID = std::this_thread::get_id();
	InitTaskManager(&g_TaskManager);
}

void __AssetCalledByMainThread() {
	assert(std::this_thread::get_id() == g_MainThreadID);
}

void TerminateTaskManager (TaskManager* manager) {
	static auto flushWorkQueue = [](WorkQueue* queue) {
		queue->mutex.lock();

		for (auto task : queue->tasksToExecute) delete task;
		for (auto task : queue->tasksToFinialize) delete task;
		queue->tasksToExecute.clear();
		queue->tasksToFinialize.clear();

		queue->condition_variable.notify_all();
		queue->mutex.unlock();
	};

	flushWorkQueue(&manager->workQueue);
	for (U32 i = 0; i < manager->threads.size(); i++) {
		manager->threads[i].join();
	}

	manager->threads.clear();
}

void TerminateTaskManager() {
	TerminateTaskManager(&g_TaskManager);
}

// DONT SUBMIT NEW WORK IN A FINALIZER!
//void SubmitTask(std::function<void(U32 workerID)> execute, std::function<void(U32 workerID)> finalize) {
//	WorkQueue* queue = &g_TaskManager.workQueue;
//
//	queue->mutex.lock();
//	Work work = {};
//	work.execute = execute;
//	work.finalize = finalize;
//	work.workerWhoCompletedMe = -1;
//	queue->workToExecute.push_back(work);
//	queue->condition_variable.notify_one();
//	queue->mutex.unlock();
//}

void FinializeCompletedTasks() {
	WorkQueue* queue = &g_TaskManager.workQueue;

	queue->mutex.lock();
	for (U32 i = 0; i < queue->tasksToFinialize.size(); i++) {
		ITask* task = queue->tasksToFinialize[i];
		task->finalize(task->executorWorkerID);
		delete task;
	}

	queue->tasksToFinialize.clear();
	queue->mutex.unlock();
}

U32 GetTaskWorkerCount() {
	U32 result = std::thread::hardware_concurrency() - 1;
	return result;
}

TaskManager* __InternalGetTaskManager() {
	return &g_TaskManager;
}