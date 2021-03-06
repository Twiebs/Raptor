#include <Utils/Profiler.hpp>

#ifdef PROFILER_ENABLED
#define ProfilerBeginPersistantEntry(name) __ProfilerBeginPersistantEntry(name)
#define ProfilerEndPersistantEntry(name) __ProfilerEndPersistantEntry(name)
#define ProfilerBeginEntry(name) __ProfilerBeginEntry(name)
#define ProfilerEndEntry(name) __ProfilerEndEntry(name)
#define ProfilerReset() __ProfilerReset();
#else
#define ProfilerBeginPersistantEntry(name)
#define ProfilerEndPersistantEntry(name)
#define ProfilerBeginEntry(name)
#define ProfilerEndEntry(name)
#define ProfilerReset()
#endif

Profiler* GetGlobalProfiler();

#ifdef IMPLEMENT_GLOBAL_PROFILER
#undef IMPLEMENT_GLOBAL_PROFILER
global_variable Profiler g_Profiler;
Profiler* GetGlobalProfiler() { return & g_Profiler; }
void __ProfilerBeginEntry(const char* name) { g_Profiler.BeginEntry(name); }
void __ProfilerEndEntry(const char* name) { g_Profiler.EndEntry(name); }
void __ProfilerBeginPersistantEntry(const char* name) { g_Profiler.BeginPersistantEntry(name); }
void __ProfilerEndPersistantEntry(const char* name) { g_Profiler.EndPersistantEntry(name); }
void __ProfilerReset() { g_Profiler.activePersistantEntries.clear(); }
#endif // IMPLEMENT_GLOBAL_PROFILER
