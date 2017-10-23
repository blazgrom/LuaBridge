#-----------------------------
#   Enable testing
#-----------------------------
enable_testing()  

#-----------------------------
#   Initial setup
#-----------------------------
find_package(Threads REQUIRED)
include(ExternalProject)

#----------------------------------------
#   Download and "install" GoogleTest
#----------------------------------------
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

#---------------------------------------------------
#   Get GTest source and binary directories 
#   from CMake project
#---------------------------------------------------
ExternalProject_Get_Property(gtest source_dir binary_dir)


#--------------------------------------------
#   Create a libgtest target to be used as 
#   a dependency by test programs and set 
#   libgtest properties
#---------------------------------------------
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)
set_target_properties(libgtest PROPERTIES
"IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
#----------------------------------------------
#
#
#
#---------------------------------------------

add_library(libgtest_main IMPORTED STATIC GLOBAL)
add_dependencies(libgtest_main gtest)
set_target_properties(libgtest_main PROPERTIES
"IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest_main.a"
"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)


#---------------------------------------------
#   Create a libgmock target to be used 
#   as a dependency by test programs and set 
#   libgmock properties
#---------------------------------------------
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)
set_target_properties(libgmock PROPERTIES
"IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

#---------------------------------
#   Add include directories
#---------------------------------
include_directories("${source_dir}/googletest/include" "${source_dir}/googlemock/include")
