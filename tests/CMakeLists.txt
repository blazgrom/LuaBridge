cmake_minimum_required(VERSION 3.6)
set (PROJECT_TEST_EXECUTABLE_NAME luabz_test)
if(ENABLE_TEST)
    file (GLOB ALL_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp")
    add_executable(${PROJECT_TEST_EXECUTABLE_NAME} ${ALL_TEST_SOURCES})
    target_link_libraries(${PROJECT_TEST_EXECUTABLE_NAME} ${PROJECT_NAME} ${LUA_LIBRARIES} libgtest libgtest_main libgmock)
    
endif()