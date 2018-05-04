cmake_minimum_required(VERSION 3.6)
find_package(Lua REQUIRED) 
find_package(Threads REQUIRED)
find_package(benchmark REQUIRED)

set (PROJECT_BENCHMARK luabz_benchmark)
add_executable(${PROJECT_BENCHMARK} benchmark.cpp)
set_target_properties(${PROJECT_BENCHMARK} PROPERTIES
                        CXX_STANDARD 14
                        CXX_STANDARD_REQUIRED YES
                        CXX_EXTENSIONS NO
                    )
target_include_directories(${PROJECT_BENCHMARK} PUBLIC "${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/utilitybz" ${benchmark_INCLUDE_DIRS}  ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_BENCHMARK}  ${LUA_LIBRARIES}  ${benchmark_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )