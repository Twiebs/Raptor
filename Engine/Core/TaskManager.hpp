#pragma once

#include<queue>
#include<vector>
#include<iostream>
#include<thread>
#include<functional>
#include<condition_variable>
#include<atomic>
#include<memory>

#include<Core\Common.hpp>

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
	virtual void Run(uint32 threadID) = 0;
	virtual void Finalize(uint32 threadID) = 0;
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
			tasksInManager++;
			scheduledTasks.push(std::make_unique<T>(args...));
			mutex.unlock();
			condition.notify_one();
		#endif
		#ifndef MULTI_THREADED
			T task(args...);
			task.Run(0);
			task.Finalize(0);
		#endif
	}
	

	void BuildTask(std::function<void()>&& taskBegin) {
		queuedTasks.emplace(taskBegin);
	}

	void CancelTask(TaskID id) {
#ifndef MULTI_THREADED
#endif

	}

	void FinishAllTasksNow();
	void Update(double deltaTime);

private:
	std::vector<std::thread> threads;
	std::queue<std::unique_ptr<ITask>> scheduledTasks;
	std::queue<std::unique_ptr<ITask>> compleatedTasks;

	std::queue<std::function<void()>> queuedTasks;
	std::queue<std::function<void()>> finishedTasks;

	std::mutex mutex;
	std::condition_variable condition;
	uint32 tasksInManager = 0;
	bool running = true;

	//std::queue<std::function<void>> testTaskFunctorQueue;

	friend static void ThreadProc(uint32 threadID, TaskManager* manager);
};

