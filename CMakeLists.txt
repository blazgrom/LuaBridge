cmake_minimum_required(VERSION 3.6)
set (PROJECT_NAME luabz)
project(${PROJECT_NAME})
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

#--------------------
#       Options
#--------------------
option(ENABLE_TEST "Build all tests." ON) 
option(ENABLE_DOC "Build documentation with doxygen" ON)
option(ENABLE_CLANG_FORMAT "Enable clang-format code formatting" ON)
option(USE_CPP_EXCEPTIONS "Use cpp exceptions in case of error, if set to OFF uses asserts in debug and logs the error to a file in release mode" OFF)
option(ENABLE_CPPCHECK "Create cppcheck target" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analizer" OFF) 
option(ENABLE_CODE_COVERAGE "Enable code coverage with lcov" OFF)


#---------------------
#       Definitions
#---------------------
if(USE_CPP_EXCEPTIONS)
        add_definitions(-DLUABZ_USE_CPP_EXCEPTIONS)
endif()
#The following definition is used in order to construct the correct path
#for the folder contain all lua scripts required by the tests, see lua_test_helpers.hpp
add_definitions(-DLUABZ_PROJECT_FOLDER_PATH="${CMAKE_SOURCE_DIR}")
#--------------------------
#       Output locations
#--------------------------
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(REPORTS_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/reports)

#--------------------------
#       Export compile commands
#--------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#--------------------------
#       Include folder
#--------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#------------------------------------------
#       Google Test - Google Mock setup
#------------------------------------------
if(ENABLE_TEST)     
        include(googletestmock)
endif()

#-------------------------
#       CppCheck
#------------------------
if(ENABLE_CPPCHECK)
        include(cppcheck)
endif()

#--------------------
#   Doxygen
#--------------------
if(ENABLE_DOC)
        include(doxygen)
endif()

#----------------------
#       Clang tidy
#----------------------
if(ENABLE_CLANG_TIDY)
        include(clang-tidy)
endif()

#----------------------
#       Code coverage
#----------------------
if(ENABLE_CODE_COVERAGE)
        include(codecoverage)
endif()

#-----------------------
#       Clang format
#-----------------------
if(ENABLE_CLANG_FORMAT)
        include(clang-format)
endif()

#----------------------
#       Git Submodules
#----------------------
include_directories(Utils)


#----------------------
# External Libraries
#----------------------
find_package(Lua51 REQUIRED) 
include_directories(${LUA_INCLUDE_DIR})


#----------------------
# Others
#----------------------

add_subdirectory(src)
add_subdirectory(tests)