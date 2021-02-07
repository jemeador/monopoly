# CMakeList.txt : CMake project for MonopolyCmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_subdirectory (lib/Catch2)

add_executable (Test_Monopoly "TestProperty.cpp" "Test.cpp" "TestMovement.cpp")

target_link_libraries (Test_Monopoly PRIVATE MonopolyEngine)
target_link_libraries (Test_Monopoly PRIVATE Catch2::Catch2)

include_directories (Test_Monopoly "../src")
include_directories (Test_Monopoly "lib/Catch2/src")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/Catch2/contrib/")

include("CTest")
include("Catch")
catch_discover_tests(Test_Monopoly "Test*")
