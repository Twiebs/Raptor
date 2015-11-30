include_directories(../src)

function(add_test name sources)
	add_definitions(-DPROJECT_SOURCE_DIRECTORY="${CMAKE_SOURCE_DIR}")
	add_executable(${name} ${sources})
	target_link_libraries(${name} debug RaptorEngine ${LIBS} ${DEBUG_LIBS})
	target_link_libraries(${name} optimized RaptorEngine ${LIBS} ${RELEASE_LIBS})
endfunction(add_test)

add_executable(WorldTest WorldTest.cpp)
add_executable(SpriteTest SpriteTest.cpp)
add_executable(FowardShadingTest FowardShadingTest.cpp)
add_executable(ECSTest ECSTest.cpp)

target_link_libraries(FowardShadingTest RaptorEngine ${LIBS})
target_link_libraries(SpriteTest RaptorEngine ${LIBS})
target_link_libraries(WorldTest RaptorEngine ${LIBS})
target_link_libraries(ECSTest RaptorEngine ${LIBS})

add_test(CubemapTest cubemap.cpp)
add_test(LiquidTest liquid.cpp)
add_test(PlatformerTest PlatformerTest.cpp)
add_test(SSAO ssao/main.cpp)
add_test(LandscapeTest landscape.cpp)

add_executable(AssetTest AssetTest.cpp)

