find_program(CPPCHECK_PATH cppcheck)
if(CPPCHECK_PATH)
    message(STATUS "cppcheck - static analysis                          YES ")
    list(APPEND CPPCHECK_ARGS
    --enable=all
    --std=c++11
    --language=c++
    --xml-version=2
    --force
    --suppress=missingIncludeSystem
    --library=windows,posix,gnu
    -I ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/*/*.h
    ${CMAKE_SOURCE_DIR}/include/*/*.hpp    
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*/*.cpp
    2> result.xml &&
    cppcheck-htmlreport
    --source-encoding="iso8859-1"
    --title=${PROJECT_NAME}
    --source-dir= ${CMAKE_SOURCE_DIR}/include/* ${CMAKE_SOURCE_DIR}/src/*
    --report-dir=${REPORTS_OUTPUT_PATH}/cppcheck
    --file=result.xml && rm result.xml)
    add_custom_target(
            cppcheck
            ${CPPCHECK_PATH} ${CPPCHECK_ARGS} 
            COMMENT "Running cppcheck and generating report")
else()
    message(STATUS "cppcheck - static analysis                              NO")
endif()