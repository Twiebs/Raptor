#pragma once

#include<queue>
#include<vector>
#include<iostream>
#include<thread>
#include<functional>
#include<condition_variable>
#include<atomic>
#include<memory>

#include<Core\Config.hpp>
#include<Core\Preprocess.hpp>
#include<Core\Types.h>

//TODO Were going to need some sort of handle for the tasks incase we need to interupt them...
typedef uint64 TaskID;

template<typename T, uint32 size>
class ConcurentQueue{
public:
	ConcurentQueue();
	~ConcurentQueue();

	// Attempts to add a item to the queue
	// Returns true if the add was suscuessful
	// False if it was not
	bool TryPush(const T& item) {
		int write = writePos.load(std::memory_order_relaxed);	// Load the writePosition
		if (write > size) return false;							// Insure that we still have room in the queue
		items[write] = item;									// Set the element at the write index
		writePos.store(write + 1, memory_order_release);
		return true;
	}

	// Attempts to pop an item from the concurentQueue
	bool TryPop(T& item) {
		int write = writePos.load(std::memory_order_acquire);
		if (readPos >= write) return false;
		item = items[readPos];
		readPos++;
		return true;
	}

private:
	T items[size];
	std::atomic<uint32> writePos;
	uint32 readPos;
};


class ITask {
public:
	virtual void Run() = 0;
	virtual void Finalize() = 0;
};

// Idealy this should be the only layer of the engine that knows anything about
// Multicore opperations
class TaskManager {
public:
	TaskManager();
	~TaskManager();

	void Startup();
	void Shutdown();
	
	//TODO return a taskID for iterupt capabilities
	//Schedules a new task and adds it to the taskQueue
	//If single threaded simply initalizes the task and then runs it
	template<typename T, typename... Args>
	void ScheduleTask(Args... args) {
		static_assert(std::is_base_of<ITask, T>::value, "Scheduled task must be derived from the ITask interface!");
		#ifdef MULTI_THREADED
			mutex.lock();
			scheduledTasks.push(std::make_unique<T>(args...));
			mutex.unlock();
			condition.notify_one();
		#endif
		#ifndef MUTLI_THREADED
			//NOTE Do we need to even use a unique_ptr here?
			//Whats wrong with the stack?
			//Benchmark this to find out if there are any preformance considerations that might provide the anwser
			//to this question
			auto task = std::make_unique<T>(args...);
			task->Run();
			task->Finalize();
		#endif
	}

	inline static TaskManager& Instance() {
		static TaskManager* instance = nullptr;
		if (instance == nullptr)
			instance = new TaskManager();
		return *instance;
	}

	void Update();

private:
	friend static void ThreadProc(TaskManager* manager);
	std::vector<std::thread> threads;
	std::queue<std::unique_ptr<ITask>> scheduledTasks;
	std::queue<std::unique_ptr<ITask>> compleatedTasks;
	std::mutex mutex;
	std::condition_variable condition;

	bool running = true;
};

