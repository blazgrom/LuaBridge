#--------------------
#   Doxygen
#--------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen - documentation generator                YES ")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
                      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen" VERBATIM
                    )
else(DOXYGEN_FOUND)
    message(STATUS "Doxygen - documentation generator                NO ")
endif(DOXYGEN_FOUND)