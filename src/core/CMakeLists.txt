cmake_minimum_required(VERSION 3.6)
set (PROJECT_NAME luabz)
project(${PROJECT_NAME})
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Lua51 REQUIRED) 
include_directories(${LUA_INCLUDE_DIR})

#target_link_libraries(cmqhd ${LUA_LIBRARIES} ${Boost_LIBRARIES} ${GOBJECT_LIBRARIES} -lntl -lm -lgmp )


file (GLOB LUABZ_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
add_library(${PROJECT_NAME} ${LUABZ_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})
if(ENABLE_TEST AND EXISTS ${PROJECT_SOURCE_DIR}/tests)     
    set (PROJECT_TEST_NAME luabz_test)
    file (GLOB LUABZ_TEST_SOURCES "${PROJECT_SOURCE_DIR}/tests/*.cpp")
    if(NOT ${LUABZ_TEST_SOURCES})
        add_executable(${PROJECT_TEST_NAME} ${LUABZ_TEST_SOURCES})
        target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME} ${LUA_LIBRARIES} libgtest libgtest_main libgmock)
    endif()
endif()