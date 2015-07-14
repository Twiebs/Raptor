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

void TaskManager::Update(double deltaTime) {
#ifdef MULTI_THREADEDcli
	while (compleatedTasks.size() > 0) {
		mutex.lock();
		auto task = std::move(compleatedTasks.front());
		compleatedTasks.pop();
		tasksInManager--;
		mutex.unlock();
		task->Finalize(0);
	}
#endif
}

void TaskManager::FinishAllTasksNow() {
#ifdef MULTI_THREADED
	while (tasksInManager > 0) {
		Update(0);
	}
#endif
}

void TaskManager::Shutdown() {
#ifdef MULTI_THREADED
	bool stop = true;
	condition.notify_all();
	for (std::thread& thread : threads) {
		thread.join();
	}
#endif
}
