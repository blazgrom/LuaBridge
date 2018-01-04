#----------------------
#       Clang format
#----------------------
find_program(CLANG_F clang-format)
if(CLANG_F)
    message(STATUS "clang-format - code formatting                      YES")
    file(GLOB_RECURSE
    ALL_CXX_SOURCE_FILES
    *.[chit]pp *.inl
    )
    add_custom_target(
        clang-format
        COMMAND ${CLANG_F}
        -i
        -style=file
        ${ALL_CXX_SOURCE_FILES}
    )
else(CLANG_F)
    message(STATUS "clang-format - code formatting                      NO")
endif(CLANG_F)