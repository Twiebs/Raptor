#include "Tasks.hpp"
#include <type_traits>

CREATE_LOG_CATEGORY(TaskManager, LOGLEVEL_DISABLED);

global_variable std::thread::id g_MainThreadID;

struct LambdaTask : public ITask {
	std::function<void()> execute_lambda;
	std::function<void()> finalize_lambda;
	virtual void execute(U32 workerID) final;
	virtual void finalize(U32 workerID) final;
	LambdaTask(const std::function<void()>& execute, const std::function<void()>& finalize);
};

LambdaTask::LambdaTask(const std::function<void()>& execute, const std::function<void()>& finalize) : 
	execute_lambda(execute), finalize_lambda(finalize) {}

void LambdaTask::execute(U32 workerID) { execute_lambda(); }
void LambdaTask::finalize(U32 workerID) { finalize_lambda(); }

static void ThreadProc(WorkQueue* queue, U32 workerID) {
	bool isRunning = true;

	ITask* task;
	bool taskNeedsExecution = false;

	while (isRunning) {
		if (taskNeedsExecution) {
			task->execute(workerID);
			task->executorWorkerID = workerID;
			taskNeedsExecution = false;

			std::lock_guard<std::mutex> lock(queue->mutex);
			queue->tasksToFinialize.push_back(task);
		}


		std::unique_lock<std::mutex> lock(queue->mutex);
		LOGERROR(TaskManager, "workerID is waiting for work!");
		LOG_VERBOSE("Worker " << workerID << " waiting for work...");
		queue->condition_variable.wait(lock);
		LOG_VERBOSE("Worker " << workerID << " executing work...");

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

void InitTaskManager (TaskManager* manager) {
	g_MainThreadID = std::this_thread::get_id();
	auto threadCount = GetTaskWorkerCount();
	manager->threads.resize(threadCount);

	for (U32 i = 0; i < threadCount; i++) {
		manager->threads[i] = std::thread(ThreadProc, &manager->workQueue, i);
	}
}


void TerminateTaskManager (TaskManager* manager) {
	static auto flushWorkQueue = [](WorkQueue* queue) {
		std::lock_guard<std::mutex> lock(queue->mutex);
		for (auto task : queue->tasksToExecute) delete task;
		for (auto task : queue->tasksToFinialize) delete task;
		queue->tasksToExecute.clear();
		queue->tasksToFinialize.clear();
		queue->condition_variable.notify_all();
	};

	flushWorkQueue(&manager->workQueue);
	for (U32 i = 0; i < manager->threads.size(); i++) {
		manager->threads[i].join();
	}

	manager->threads.clear();
}

void FinializeCompletedTasks (WorkQueue* queue) {

	std::vector<ITask*> tasksToFinalize;
	{ 
		std::lock_guard<std::mutex> lock(queue->mutex);
		if (queue->tasksToFinialize.size() > 0) { LOG_VERBOSE("Main thread is gathering compleated tasks"); }
		for (U32 i = 0; i < queue->tasksToFinialize.size(); i++) {
			tasksToFinalize.emplace_back(queue->tasksToFinialize.back());
			queue->tasksToFinialize.pop_back();
		}
	}

	for (auto task : tasksToFinalize) {
		LOG_VERBOSE("Main thread is finalizing a task!");
		task->finalize(0);
	}
}

U32 GetTaskWorkerCount() {
	U32 result = std::thread::hardware_concurrency() - 1;
	return result;
}

void __assertCalledByMainThread() {
	assert(std::this_thread::get_id() == g_MainThreadID &&
		"This function must be called from the main thread!");
}