cmake_minimum_required(VERSION 3.13)

project(csvm-tests)

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(test-${test_name} ${test_src})
    target_link_libraries(test-${test_name} PRIVATE csvm Catch2::Catch2WithMain)
    add_test(NAME test-${test_name} COMMAND test-${test_name})
endforeach()
