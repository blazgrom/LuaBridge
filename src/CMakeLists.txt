cmake_minimum_required(VERSION 3.6)
find_package(Lua REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES
                        CXX_STANDARD 14
                        CXX_STANDARD_REQUIRED YES
                        CXX_EXTENSIONS NO
                    )
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/utilitybz/include"  ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  ${LUA_LIBRARIES} )
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wshadow -pedantic)