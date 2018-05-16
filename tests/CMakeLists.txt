cmake_minimum_required(VERSION 3.6)
find_package(Lua REQUIRED) 

set (PROJECT_TESTS luabz_tests)
file (GLOB ALL_TEST_SOURCES core/*.cpp
                            detail/*.cpp)
add_executable(${PROJECT_TESTS} ${ALL_TEST_SOURCES})
set_target_properties(${PROJECT_TESTS} PROPERTIES
                        CXX_STANDARD 14
                        CXX_STANDARD_REQUIRED YES
                        CXX_EXTENSIONS NO
                    )
target_include_directories(${PROJECT_TESTS} PUBLIC "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/utilitybz/include"  ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_TESTS}  ${LUA_LIBRARIES}  libgtest libgtest_main)