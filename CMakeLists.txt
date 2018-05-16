cmake_minimum_required(VERSION 3.6)
set (PROJECT_NAME luabz)
project(${PROJECT_NAME})
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow  -pedantic ")
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

option(ENABLE_DOC "Build documentation with doxygen" OFF)
option(USE_CPP_EXCEPTIONS "Use cpp exceptions in case of error, if set to OFF uses asserts in debug and logs the error to a file in release mode" OFF)
option(ENABLE_CPPCHECK "Create cppcheck target" OFF)
option(ENABLE_CODE_COVERAGE "Enable code coverage with lcov" OFF)


if(USE_CPP_EXCEPTIONS)
        add_definitions(-DLUABZ_USE_CPP_EXCEPTIONS)
endif()


#The following definition is used in order to construct the correct path
#for the folder contain all lua scripts required by the tests, see lua_test_helpers.hpp
add_definitions(-DLUABZ_PROJECT_FOLDER_PATH="${CMAKE_SOURCE_DIR}")


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(REPORTS_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/reports)

include(googletestmock)
include(clang-format)
include(clang-tidy)
include(cppcheck)
include(iwyu)

if(ENABLE_DOC)
        include(doxygen)
endif()


if(ENABLE_CODE_COVERAGE)
        include(codecoverage)
endif()




add_subdirectory(tests)
#Add support for iwyu
#Replace googletest with catch