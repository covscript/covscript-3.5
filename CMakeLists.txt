cmake_minimum_required(VERSION 3.13)

project(csvm)

set(CMAKE_CXX_STANDARD 17)

option(CSVM_ENABLE_TESTS "Build with unit tests" ON)

add_subdirectory(third-party)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sources/**/*.cpp")

add_library(csvm STATIC ${SOURCES})
target_include_directories(csvm PUBLIC include)
target_include_directories(csvm PUBLIC third-party)

if (MSVC)
    target_compile_options(csvm PUBLIC /O2 /EHsc /utf-8 /DNDEBUG /Zc:__cplusplus /MP /w)
    set_target_properties(csvm PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_RC_FLAGS "/nologo /c65001")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(csvm PUBLIC -O2 -DNDEBUG -fPIC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(csvm PUBLIC -Wno-interference-size)
    endif ()
    if (WIN32)
        target_compile_options(csvm PUBLIC --static)
    elseif (UNIX AND NOT APPLE)
        target_compile_options(csvm PUBLIC -fno-plt)
    endif ()
endif ()

if (CSVM_COMPATIBILITY_MODE)
    message(STATUS "CovScript: Configuring Compatibility Mode")
    target_compile_definitions(csvm PUBLIC CSVM_COMPATIBILITY_MODE)
endif ()

if(CSVM_ENABLE_TESTS)
    message(STATUS "CovScript: Build with unit tests")
    add_subdirectory(third-party/catch2)
    add_subdirectory(tests/cpp)
endif()
