#include "TaskManager.hpp"


TaskManager::TaskManager() {
	Startup();
}


TaskManager::~TaskManager() {
	
}

void TaskManager::Startup() {
	auto threadCount = std::thread::hardware_concurrency();
	for (auto i = 0; i < threadCount; i++)
		threads.emplace_back(ThreadProc, this);
}

static void ThreadProc(TaskManager* manager) {
	while (true) {
		Task task = nullptr;
		{
			std::unique_lock<std::mutex> lock(manager->mutex);
			manager->condition.wait(lock);
			if (!manager->running) return;
			if (manager->tasks.size() > 0) {
				task = std::move(manager->tasks.front());
				manager->tasks.pop();
			}
		}
		task();
	}
}

void TaskManager::Shutdown() {
	bool stop = true;
	condition.notify_all();
	for (std::thread& thread : threads) {
		thread.join();
	}
}

void TaskManager::AddTask(Task task) {
	mutex.lock();
	tasks.push(task);
	mutex.unlock();
	condition.notify_one();
}