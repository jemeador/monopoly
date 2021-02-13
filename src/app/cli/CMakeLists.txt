# CMakeList.txt : CMake project for MonopolyCmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's library.
file (GLOB SOURCES *.cpp *.h)

# Add source to this project's executable.
add_executable (MonopolyCli ${SOURCES})
include_directories (MonopolyCli "../.." ".")
target_link_libraries (MonopolyCli MonopolyEngine)

install (
	TARGETS MonopolyCli
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

