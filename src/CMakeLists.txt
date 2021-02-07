# CMakeList.txt : CMake project for MonopolyCmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's library.
file (GLOB SOURCES *.cpp)
add_library (MonopolyEngine ${SOURCES} "GameState.h" "GameState.cpp" "Input.h")

# Add source to this project's executable.
add_executable (Monopoly app/main.cpp "GameState.h" "GameState.cpp" "Input.h")
include_directories (Monopoly ".")
target_link_libraries (Monopoly MonopolyEngine)

file (GLOB HEADERS *.h)
set_target_properties (MonopolyEngine PROPERTIES PUBLIC_HEADER "${HEADERS}")
INSTALL (
	TARGETS MonopolyEngine
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

