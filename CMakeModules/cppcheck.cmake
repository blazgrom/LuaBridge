find_program(CPPCHECK_PATH cppcheck)
if(CPPCHECK_PATH)
    message(STATUS "cppcheck - static analysis                YES ")
    list(APPEND CPPCHECK_ARGS
    --enable=warning,style,performance,portability,unusedFunction
    --std=c++11
    --verbose
    --language=c++
    -I ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/*/*.h
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*/*.cpp)

    add_custom_target(
            cppcheck
            COMMAND ${CPPCHECK_PATH} ${CPPCHECK_ARGS} 
            COMMENT "running cppcheck" )
else()
    message(STATUS "cppcheck - static analysis                NO")
endif()