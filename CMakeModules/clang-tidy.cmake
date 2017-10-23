if(ENABLE_CLANG_TIDY)
find_program(CLANG_TIDY NAMES clang-tidy)
if(CLANG_TIDY)
    message(STATUS "clang-tidy - static analysis                YES ")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    file( GLOB ALL_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*/*.cpp ${CMAKE_SOURCE_DIR}/include/*/*.hpp )
    add_custom_target(
            clang-tidy
            COMMAND ${CLANG_TIDY}
            ${ALL_SOURCE_FILES}
            -checks=*
            -export-fixes='clang-tidy-fixes.dat'
            -p=.)
else(CLANG_TIDY)
    message(STATUS "clang-tidy - static analysis                NO ")
endif(CLANG_TIDY)
endif(ENABLE_CLANG_TIDY)