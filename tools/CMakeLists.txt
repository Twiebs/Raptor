include_directories(../src)
add_executable(AssetExplorer AssetExplorer.cpp)
target_link_libraries(AssetExplorer RaptorEngine ${LIBS})