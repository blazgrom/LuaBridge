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
option(ALWAYS_BUILD_DOC "Build the documentation on every make" OFF)
option(ENABLE_CPPCHECK "Create cppcheck target" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analizer" OFF) 
option(ENABLE_CODE_COVERAGE "Enable code coverage with lcov" OFF)


#--------------------------
#       Output locations
#--------------------------
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

#--------------------------
#       Include folder
#--------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)

#------------------------------------------
#       Google Test - Google Mock setup
#------------------------------------------
if(ENABLE_TEST)     
        #This request Threads and threads genered CMakeFiles folder in the bin folder...
        include(googletestmock)
        enable_testing()        
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
#       Clang tidy
#----------------------
if(ENABLE_CODE_COVERAGE)
        ##TODO : Add script for test coverage
endif()

#----------------------
#       Submodules
#----------------------
include_directories(Utils)


#----------------------
# External Libraries
#----------------------
find_package(Lua51 REQUIRED) 
include_directories(${LUA_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(tests)