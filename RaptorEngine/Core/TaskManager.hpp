#pragma once

#include<queue>
#include<vector>
#include<iostream>
#include<thread>
#include<functional>
#include<condition_variable>

#include<Core\Config.hpp>
#include<Core\Preprocess.hpp>

#ifdef MULTI_THREADED
#define SCHEDULE_TASK(x) TaskManager::Instance().AddTask(x)
#endif
#ifndef MULTI_THREADED
#define SCHEDULE_TASK(x) x()
#endif

typedef void(*Task)(void);

class TaskManager {
public:
	TaskManager();
	~TaskManager();

	void Startup();
	void Shutdown();
	
	void AddTask(Task task);

	inline static TaskManager& Instance() {
		static TaskManager* instance = nullptr;
		if (instance == nullptr)
			instance = new TaskManager();
		return *instance;
	}

private:
	friend static void ThreadProc(TaskManager* manager);
	std::vector<std::thread> threads;
	std::queue<Task> tasks;

	std::mutex mutex;
	std::condition_variable condition;

	bool running = true;
};

