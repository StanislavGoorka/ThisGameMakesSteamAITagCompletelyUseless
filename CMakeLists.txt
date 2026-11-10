cmake_minimum_required(VERSION 3.26)
project(ClosedCircuit LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

add_library(game_core INTERFACE)
target_compile_features(game_core INTERFACE cxx_std_20)

include(CTest)
if(BUILD_TESTING)
    add_executable(core_tests tests/unit/build_tests.cpp)
    target_link_libraries(core_tests PRIVATE game_core)
    if(MSVC)
        target_compile_options(core_tests PRIVATE /W4 /permissive-)
    endif()
    add_test(NAME core_build_smoke COMMAND core_tests)
endif()
