# CMakeList.txt : CMake project for MonopolyCmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory (lib/json)

# Add source to this project's library.
file (GLOB SOURCES *.cpp)
add_library (MonopolyEngine ${SOURCES})
include_directories (MonopolyEngine "lib/json/include")
target_link_libraries (MonopolyEngine PRIVATE nlohmann_json::nlohmann_json)

# Add source to this project's executable.
file (GLOB HEADERS *.h)
set_target_properties (MonopolyEngine PROPERTIES PUBLIC_HEADER "${HEADERS}")

install (
	TARGETS MonopolyEngine
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

set(DATA_DIR "share/")

# Install data file for JSON display strings
message ("CMAKE_SYSTEM_NAME=" ${CMAKE_SYSTEM_NAME})
if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    # Allow empscripten to preload the JSON display strings
    message ("Emscripten target system detected; setting data path to point virtual file system")
    set(DATA_INSTALL_PREFIX ${DATA_DIR})
    # em++ flag --embed-file set globally in CMakeSettings.json so it applies to all .js files
	# Custom target to generate a JS module with C++ bindings
	add_custom_target(MonopolyJSEngine
		ALL BYPRODUCTS monopoly.js
		COMMENT "Generating JS/C++ bindings: monopoly.js"
		COMMAND ${CMAKE_CXX_COMPILER} --bind -o monopoly.js -Wl,--whole-archive libMonopolyEngine.a -Wl,--no-whole-archive
		DEPENDS libMonopolyEngine.a
	)
else ()
    set(DATA_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/${DATA_DIR}")
    message ("Setting install path for data files to " ${DATA_INSTALL_PREFIX})
    install (FILES ${DATA_DIR}/strings.json DESTINATION ${DATA_INSTALL_PREFIX})
endif ()


add_compile_definitions(
	DATA_INSTALL_PREFIX="${DATA_INSTALL_PREFIX}"
)

add_subdirectory (app/cli)
