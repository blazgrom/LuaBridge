find_program(CLANG_TIDY NAMES clang-tidy)
if(CLANG_TIDY)
    message(STATUS "clang-tidy - static analysis                YES ")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    file( GLOB ALL_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/main.cpp )
    add_custom_target(
            clang-tidy
            COMMAND ${CLANG_TIDY}
            ${ALL_SOURCE_FILES}
            -checks=-*,bugprone-*,cppcoreguidelines-*,clang-analyzer-cplusplus*,misc-*,modernize-*,performance-*,readability-*,portability-*
           # -warnings-as-errors=-*,bugprone-*,cppcoreguidelines-*,clang-analyzer-cplusplus*,misc-*,modernize-*,performance-*,readability-*,portability-*
            -header-filter=.*
            -p=.)
else(CLANG_TIDY)
    message(STATUS "clang-tidy - static analysis                NO ")
endif(CLANG_TIDY)