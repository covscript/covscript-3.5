cmake_minimum_required(VERSION 3.13)

project(covscript)

set(CMAKE_CXX_STANDARD 17)

option(COVSCRIPT_ENABLE_TESTS "Build with unit tests" ON)

add_subdirectory(third-party)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sources/**/*.cpp")

add_library(covscript STATIC ${SOURCES})
target_include_directories(covscript PUBLIC include)
target_include_directories(covscript PUBLIC third-party)

if (MSVC)
    target_compile_options(covscript PUBLIC /O2 /EHsc /utf-8 /DNDEBUG /Zc:__cplusplus /MP /w)
    set_target_properties(covscript PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_RC_FLAGS "/nologo /c65001")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(covscript PUBLIC -O2 -DNDEBUG -fPIC)
    if (WIN32)
        target_compile_options(covscript PUBLIC --static)
    elseif (UNIX AND NOT APPLE)
        target_compile_options(covscript PUBLIC -fno-plt)
    endif ()
endif ()

if (CS_COMPATIBILITY_MODE)
    message(STATUS "CovScript: Configuring Compatibility Mode")
    target_compile_definitions(covscript PUBLIC CS_COMPATIBILITY_MODE)
endif ()

if(COVSCRIPT_ENABLE_TESTS)
    message(STATUS "CovScript: Build with unit tests")
    add_subdirectory(third-party/catch2)
    add_subdirectory(tests/cpp)
endif()
