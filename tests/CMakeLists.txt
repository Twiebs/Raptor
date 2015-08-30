include_directories(../src)
add_executable(WorldTest WorldTest.cpp )
target_link_libraries(WorldTest RaptorEngine ${LIBS})
