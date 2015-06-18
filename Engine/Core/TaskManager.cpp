#include "TaskManager.hpp"


TaskManager::TaskManager() {
	Startup();
}


TaskManager::~TaskManager() {
	
}

void TaskManager::Startup() {
#ifdef MULTI_THREADED
	auto threadCount = std::thread::hardware_concurrency();
	if (threadCount < MINIMUM_THREADS)
		threadCount = MINIMUM_THREADS;

	for (uint32 i = 0; i < threadCount; i++)
		threads.emplace_back(ThreadProc, i, this);
#endif
}

static void ThreadProc(uint32 threadID, TaskManager* manager) {
	for(;;) {
		std::unique_ptr<ITask> task = nullptr;
		{
			std::unique_lock<std::mutex> lock(manager->mutex);
			manager->condition.wait(lock);
			if (!manager->running) return;
			if (manager->scheduledTasks.size() > 0) {
				task = std::move(manager->scheduledTasks.front());
				manager->scheduledTasks.pop();
			}
		}
		task->Run(threadID);
		manager->mutex.lock();
		manager->compleatedTasks.emplace(std::move(task));
		manager->mutex.unlock();
	}
}

void TaskManager::Update(double deltaTime) {
	while (compleatedTasks.size() > 0) {
		mutex.lock();
		auto task = std::move(compleatedTasks.front());
		compleatedTasks.pop();
		tasksInManager--;
		mutex.unlock();
		task->Finalize(0);
	}
}

void TaskManager::FinishAllTasksNow() {
#ifdef MULTI_THREADED
	while (tasksInManager > 0) {
		Update(0);
	}
#endif
}

void TaskManager::Shutdown() {
	bool stop = true;
	condition.notify_all();
	for (std::thread& thread : threads) {
		thread.join();
	}
}
