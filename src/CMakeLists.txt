
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Raptor)
set(CMAKE_CXX_STANDARD 14)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
if(NOT MSVC)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -I/usr/include/freetype2)
endif(NOT MSVC)

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



if(UNIX)
  set(RAPTOR_LIBS
      -lGL
      -lGLEW
      -lglfw
      -lfreetype
      -lfreeimage
      -lassimp
  )
endif(UNIX)



#==============================================
#   Build Options
#==============================================
option(RAPTOR_GLFW FALSE)
option(RAPTOR_SDL FALSE)
option(RAPTOR_HTML FALSE)
#==============================================
#----------------------------------------------
#==============================================

include_directories(Engine)
include_directories(../include)
if(MSVC)
  link_directories(../lib)
  include_directories(../include/freetype)
  SET(RAPTOR_LIBS
    opengl32.lib
    assimpd.lib
    FreeImage.lib
    freetype.lib
    SDL2.lib
    SDL2main.lib
    SDL2test.lib
    glew32.lib
    glfw3.lib
  )
endif(MSVC)


#Build the RaptorEngine as a static lib
#The below directories are used to create a list of files to pass to the add_library function
set(ENGINE_SRC_DIRS Application Assets Components Core Graphics Graphics/GL Math Memory Platform Systems UI Utils)
foreach(DIR ${ENGINE_SRC_DIRS})
  file(GLOB ENGINE_${DIR}_CPP Engine/${DIR}/*.cpp)
  file(GLOB ENGINE_${DIR}_HPP Engine/${DIR}/*.hpp)
  set(ENGINE_${DIR}_SRCS ${ENGINE_${DIR}_CPP} ${ENGINE_${DIR}_HPP})
  list(APPEND ENGINE_ALL_SRCS ${ENGINE_${DIR}_SRCS})
endforeach(DIR ${ENGINE_SRC_DIRS})

#Add the sources that were appended to the library
add_library(RaptorCore STATIC ${ENGINE_ALL_SRCS})

#Generate MSVS Filters for our files
foreach(DIR ${ENGINE_SRC_DIRS})
  source_group(${DIR} FILES ${ENGINE_${DIR}_SRCS})
endforeach(DIR ${ENGINE_SRC_DIRS})


file(GLOB TOOLS_SRCS Tools/*.cpp)
add_executable(RaptorTools ${TOOLS_SRCS})
target_link_libraries(RaptorTools RaptorCore)
set(CMAKE_LIBRARY_PATH ../lib/${CMAKE_BUILD_TYPE})
target_link_libraries(RaptorTools ${RAPTOR_LIBS})

source_group(Tools FILES ${TOOLS_SRCS})
