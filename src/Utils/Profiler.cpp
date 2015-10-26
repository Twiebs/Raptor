#include "Profiler.hpp"

#include <vector>

#include <Core/types.h>
#include <Core/Platform.h>

// We dont realy want these staticly allocated globals
// in the framework...
global_variable Profiler* g_Profiler = new Profiler();

void Profiler::BeginEntry (const char* name) {
	auto& entry = entires[newEntryIndex];
	entry.tempTime = PlatformGetPerformanceCounter();
	entry.elapsedCycles = __rdtsc();
	entry.name = name;
	entryIndexStack.push_back(newEntryIndex);
	newEntryIndex++;
}

void Profiler::EndEntry(const char* name) {
	auto index = entryIndexStack.back();
	auto& entry = entires[index];

	auto time = PlatformGetPerformanceCounter() - entry.tempTime;
	entry.elapsedTime = (double)(time * 1000) / PlatformGetPerformanceCounterFrequency();
	entry.elapsedCycles = __rdtsc() - entry.elapsedCycles;
	assert(entry.name == name);
	entryIndexStack.pop_back();
}

static void InternalProfilerBeginBlock (Profiler* profiler, const char* name) {
	ProfilerPersistantEntry* entry;
	U32 entryIndex = profiler->persistantEntryNameMap[name];
	
	if (entryIndex == 0) {
		profiler->persistantEntries.push_back(ProfilerPersistantEntry());
		entry = &profiler->persistantEntries.back();
		entry->name = name;
		entry->entryIndex = profiler->persistantEntries.size() - 1;
		profiler->persistantEntryNameMap[name] = entry->entryIndex + 1;
	} else {
		entry = &profiler->persistantEntries[entryIndex - 1];
	}

	profiler->activePersistantEntries.push_back(entry->entryIndex);
	profiler->persistantEntryIndexStack.push_back(profiler->activePersistantEntries.size() - 1);

	entry->tempTime = PlatformGetPerformanceCounter();
	entry->elapsedCycles[entry->writeIndex] = __rdtsc();
}

static void InternalProfilerEndBlock (Profiler* profiler, const char* name) {
	auto activeEntryIndex = profiler->persistantEntryIndexStack.back();
	profiler->persistantEntryIndexStack.pop_back();

	auto entryIndex = profiler->activePersistantEntries[activeEntryIndex];
	auto entry = &profiler->persistantEntries[entryIndex];

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

void __ProfilerBeginEntry(const char* name) {
	g_Profiler->BeginEntry(name);
}

void __ProfilerEndEntry(const char* name) {
	g_Profiler->EndEntry(name);
}

void __ProfilerBeginPersistantEntry(const char* name) {
	InternalProfilerBeginBlock(g_Profiler, name);
}

void __ProfilerEndPersistantEntry(const char* name) {
	InternalProfilerEndBlock(g_Profiler, name);
}

void __ProfilerReset() {
	g_Profiler->activePersistantEntries.clear();
}

Profiler* GetGlobalProfiler() {
	return g_Profiler;
}