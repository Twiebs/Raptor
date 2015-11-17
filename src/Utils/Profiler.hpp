#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <Core/types.h>

#ifndef PROFILER_PERSISTANT_SAMPLE_COUNT
#define PROFILER_PERSISTANT_SAMPLE_COUNT 64
#endif	//PROFILER_PERSISTANT_SAMPLE_COUNT
#ifndef PROFILER_INSTANTANEOUS_ENTRY_COUNT
#define PROFILER_INSTANTANEOUS_ENTRY_COUNT 128
#endif //PROFILER_INSTANTANEOUS_ENTRY_COUNT

struct ProfilerPersistantEntry {
	std::string name;
	U32 entryIndex = 0;
	U32 writeIndex = 0;
	U64 tempTime;
	float elapsedTimes[PROFILER_PERSISTANT_SAMPLE_COUNT];
	U64 elapsedCycles[PROFILER_PERSISTANT_SAMPLE_COUNT];
};

struct ProfilerEntry {
	std::string name;
	double elapsedTime;
	U64 elapsedCycles;
	U64 tempTime;
};

struct Profiler {
	std::vector<U32> persistantEntryIndexStack;
	std::vector<ProfilerPersistantEntry> persistantEntries;
	std::vector<U32> activePersistantEntries;
	std::unordered_map<std::string, U32> persistantEntryNameMap;

	U32 newEntryIndex;
	ProfilerEntry entires[PROFILER_INSTANTANEOUS_ENTRY_COUNT];
	std::vector<U32> entryIndexStack;

	void BeginEntry(const char* name);
	void EndEntry(const char* name);
	void BeginPersistantEntry(const char* name);
	void EndPersistantEntry(const char* name);
};
