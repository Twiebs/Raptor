include_directories(../src)
add_executable(MeshViewer MeshViewer.cpp)
target_link_libraries(MeshViewer RaptorEngine ${LIBS})