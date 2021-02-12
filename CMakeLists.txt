# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project ("Monopoly")


find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/src/lib/json/CMakeLists.txt")
    message(FATAL_ERROR "json was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/test/lib/Catch2/CMakeLists.txt")
    message(FATAL_ERROR "Catch2 was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# Include sub-projects.
add_subdirectory ("src")
add_subdirectory ("test")
enable_testing ()
add_test (NAME TestAll COMMAND Test_Monopoly)
