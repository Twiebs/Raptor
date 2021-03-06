#ifndef RAPTOR_CONFIG_HPP
#define RAPTOR_CONFIG_HPP

#define ASSET_DIRECTORY "../../resources/assets/"
#define SHADER_DIRECTORY "../../src/shaders/"

#define ENABLE_ASSET_RELOADING

#define PROFILER_ENABLED
#define PROFILER_PERSISTANT_SAMPLE_COUNT 64

#define COMPILE_SHADERS_AT_RUNTIME
// #define GENERATE_SHADER_SOURCE_FILES
#define GENERATE_SHADER_SOURCE_FILE_DIRECTORY "generated_shaders/"

#define MATERIAL_NAME_MAX_SIZE 64

// #define FORCE_SINGLE_THREADED
#define FORCE_MINIMUM_THREAD_COUNT 2

#define ENABLE_UNIFORM_WARNINGS

#endif // RAPTOR_CONFIG_HPP