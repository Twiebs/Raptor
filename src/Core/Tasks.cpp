#include "Tasks.hpp"

#include <thread>

struct Task {

};

static void ThreadProc() {

}

static void Execute(Task* task);

static void SpawnThreads () {
	auto threadCount = std::thread::hardware_concurrency() - 1;

}