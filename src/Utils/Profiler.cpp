#include "Profiler.hpp"

#include <vector>

#include <Core/Tasks.hpp>
#include <Core/types.h>
#include <Core/Platform.h>

void Profiler::BeginEntry (const char* name) {
	auto& entry = entires[newEntryIndex];
	entry.tempTime = PlatformGetPerformanceCounter();
	entry.elapsedCycles = __rdtsc();
	entry.name = name;
	entryIndexStack.push_back(newEntryIndex);
	newEntryIndex++;
}

void Profiler::EndEntry (const char* name) {
	auto index = entryIndexStack.back();
	auto& entry = entires[index];

	auto time = PlatformGetPerformanceCounter() - entry.tempTime;
	entry.elapsedTime = (double)(time * 1000) / PlatformGetPerformanceCounterFrequency();
	entry.elapsedCycles = __rdtsc() - entry.elapsedCycles;
	assert(entry.name == name && "You called EndEntry with a different name then you called BeginEntry with!");
	entryIndexStack.pop_back();
}

void Profiler::BeginPersistantEntry(const char* name) {
	assert_called_by_main_thread();

	ProfilerPersistantEntry* entry;
	U32 entryIndex = persistantEntryNameMap[name];

	if (entryIndex == 0) {
		persistantEntries.push_back(ProfilerPersistantEntry());
		entry = &persistantEntries.back();
		entry->name = name;
		entry->entryIndex = persistantEntries.size() - 1;
		persistantEntryNameMap[name] = entry->entryIndex + 1;
	} else {
		entry = &persistantEntries[entryIndex - 1];
	}

	activePersistantEntries.push_back(entry->entryIndex);
	persistantEntryIndexStack.push_back(activePersistantEntries.size() - 1);

	entry->tempTime = PlatformGetPerformanceCounter();
	entry->elapsedCycles[entry->writeIndex] = __rdtsc();
}

void Profiler::EndPersistantEntry(const char* name) {
	assert_called_by_main_thread();
	auto activeEntryIndex = persistantEntryIndexStack.back();
	persistantEntryIndexStack.pop_back();

	auto entryIndex = activePersistantEntries[activeEntryIndex];
	auto entry = &persistantEntries[entryIndex];

	entry->tempTime = PlatformGetPerformanceCounter() - entry->tempTime;
	double tempTime = (double)(entry->tempTime * 1000) / PlatformGetPerformanceCounterFrequency();
	entry->elapsedTimes[entry->writeIndex] = (float)tempTime;
	entry->elapsedCycles[entry->writeIndex] = __rdtsc() - entry->elapsedCycles[entry->writeIndex];

	assert(entry->name == name);
	entry->writeIndex++;
	if (entry->writeIndex > PROFILER_PERSISTANT_SAMPLE_COUNT) {
		entry->writeIndex = 0;
	}
}
