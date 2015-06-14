#include "TaskManager.hpp"


TaskManager::TaskManager() {
	Startup();
}


TaskManager::~TaskManager() {
	
}

void TaskManager::Startup() {
	auto threadCount = std::thread::hardware_concurrency();
	if (threadCount < MINIMUM_THREADS)
		threadCount = MINIMUM_THREADS;

	for (auto i = 0; i < threadCount; i++)
		threads.emplace_back(ThreadProc, this);
}

static void ThreadProc(TaskManager* manager) {
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
		task->Run();
		manager->compleatedTasks.push(std::move(task));
	}
}

void TaskManager::Update() {
	while (compleatedTasks.size() > 0) {
		mutex.lock();
		auto task = std::move(compleatedTasks.front());
		compleatedTasks.pop();
		mutex.unlock();
		task->Finalize();
	}
}

void TaskManager::Shutdown() {
	bool stop = true;
	condition.notify_all();
	for (std::thread& thread : threads) {
		thread.join();
	}
}
