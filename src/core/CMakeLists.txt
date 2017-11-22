cmake_minimum_required(VERSION 3.6)
file (GLOB PROJECT_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})