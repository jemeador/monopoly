# CMakeList.txt : CMake project for MonopolyCmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory (lib/json)

# Add source to this project's library.
file (GLOB SOURCES *.cpp)
add_library (MonopolyEngine ${SOURCES} "GameState.h" "GameState.cpp" "Input.h"  "Cards.cpp" "Strings.h" "Strings.cpp")
include_directories (MonopolyEngine "lib/json/include")
target_link_libraries (MonopolyEngine PRIVATE nlohmann_json::nlohmann_json)

# Add source to this project's executable.
add_executable (Monopoly app/main.cpp "GameState.h" "GameState.cpp" "Input.h"  "Cards.cpp" "Strings.h" "Strings.cpp")
include_directories (Monopoly ".")
target_link_libraries (Monopoly MonopolyEngine)
file (GLOB HEADERS *.h)
set_target_properties (MonopolyEngine PROPERTIES PUBLIC_HEADER "${HEADERS}")
install (
	TARGETS MonopolyEngine
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

set(DATA_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/share/")
#set(DATA_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share")

add_compile_definitions(
	DATA_INSTALL_PREFIX="${DATA_INSTALL_PREFIX}"
)
install (FILES strings.json DESTINATION ${DATA_INSTALL_PREFIX})

