
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Raptor)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
set(CMAKE_CXX_FLAGS -I/usr/include/freetype2)

#Set the paths Cmake will search for other .cmake files
set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/../CMake
    ${CMAKE_SOURCE_DIR}/../CMake/Packages
)

include(Dependencies)




# Avoid source tree pollution
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Build directory must not be the same as the source dir")
endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)


#==============================================
#   Build Options
#==============================================
option(RAPTOR_GLFW FALSE)
option(RAPTOR_SDL FALSE)
option(RAPTOR_HTML FALSE)
#==============================================
#----------------------------------------------
#==============================================

add_subdirectory(Engine)
include_directories(Engine)

add_subdirectory(Tools)

file(GLOB TOOLS_SRCS Tools/*.cpp)
add_executable(RaptorTools ${TOOLS_SRCS})
target_link_libraries(RaptorTools RaptorCore)
target_link_libraries(RaptorTools ${RAPTOR_LIBS})
