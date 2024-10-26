# Tested CMake versions:
cmake_minimum_required(VERSION 3.14...3.27)

set(TARGET_SYSTEM_LINUX TRUE)
set(TARGET_SYSTEM_WIN32 FALSE)

if (TARGET_SYSTEM_WIN32)
    # target os: Windows
    set(CMAKE_SYSTEM_NAME Windows)
    # use x86 win64 mingw32 gcc & g++ compiler
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    # path to mingw
    set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
    # don't FIND_XXX in linux
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    # only FIND_XXX in win
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

# Project name, description, language, etc.
project(untitled-game2
    DESCRIPTION "It's an untitled game"
    LANGUAGES CXX)

# Use CXX 17+.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Version numbers:
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# Testing framework.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Executable(s):
add_executable(untitled-game2)

# Target(s) features and options.
#target_compile_features(untitled-game2 PRIVATE cxx_std_20)

# Debug build compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

# Release build compiler flags.
set(CMAKE_CXX_FLAGS_RELEASE_INIT "")

# Add source files to target(s).
target_sources(untitled-game2 PRIVATE src/main.cpp
    src/utility.cpp
    src/debug.cpp
    src/app.cpp
    src/conf.cpp
    src/command.cpp
    src/command_queue.cpp
    src/creature.cpp
    src/entity.cpp
    src/game_config.h
    src/player.cpp
    src/p_task.cpp
    src/scene_node.cpp
    src/sprite_node.cpp
    src/text_node.cpp
    src/r_holders.cpp
    src/state.cpp
    src/s_stack.cpp
    src/s_title.cpp
    src/s_menu.cpp
    src/s_game.cpp
    src/s_loading.cpp
    src/s_pause.cpp
    src/s_settings.cpp
    src/world.cpp
    src/data_tables.cpp
    src/projectile.cpp
    src/pickup.cpp
    src/task-thread.cpp
    src/map-asset.cpp
    # imgui style config
    src/imguistyle.cpp
    # imgui dep
    dep/imgui/imgui.cpp
    dep/imgui/imgui_draw.cpp
    dep/imgui/imgui_demo.cpp
    dep/imgui/imgui_tables.cpp
    dep/imgui/imgui_widgets.cpp
    # imgui sfml backend dep
    dep/imgui-sfml/imgui-SFML.cpp
    )

# Add include directories to target(s).
# Local include directory.
target_include_directories(untitled-game2 PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)
# Library include directories.
target_include_directories(untitled-game2 PRIVATE
    "${CMAKE_SOURCE_DIR}/dep/imgui/")
target_include_directories(untitled-game2 PRIVATE
    "${CMAKE_SOURCE_DIR}/dep/imgui-sfml")
# Platform specific include directories.
if (TARGET_SYSTEM_LINUX)
    target_include_directories(untitled-game2 PRIVATE
        "${CMAKE_SOURCE_DIR}/dep/linux/SFML-2.6.1/include/")
    target_link_directories(untitled-game2 PRIVATE
        "${CMAKE_SOURCE_DIR}/dep/linux/SFML-2.6.1/lib/")
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/dep/linux/SFML-2.6.1/lib/cmake/SFML")

    # for linux system headers
    target_include_directories(untitled-game2 PRIVATE
        "/usr/include/")
elseif (TARGET_SYSTEM_WIN32)
    target_include_directories(untitled-game2 PRIVATE
        "${CMAKE_SOURCE_DIR}/dep/SFML-2.6.1/include/")
    target_link_directories(untitled-game2 PRIVATE
        "${CMAKE_SOURCE_DIR}/dep/SFML-2.6.1/lib/")
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/dep/SFML-2.6.1/lib/cmake/SFML")
endif()

# Find packages necessary for linking.
find_package(SFML COMPONENTS network graphics window system REQUIRED)

# Link libraries to target(s).
target_link_libraries(
	untitled-game2
	GTest::gtest_main
    #openal32
    #flac
    #vorbisenc
    #vorbisfile
    #vorbis
    #ogg
    #sfml-network
    #ws2_32
    sfml-graphics
    sfml-window
    sfml-system
    OpenGL
    GL
    #imgui
    #imgui-sfml
    #opengl32
    #freetype
    #winmm
    #gdi32
    #libimgui-sfml-d.a
    #-lOpenGL32
    #ImGui-SFML::ImGui-SFML
)

include(GoogleTest)
gtest_discover_tests(untitled-game2)

# Copy necessary directories into build directory, for executable root access.
add_custom_command(TARGET untitled-game2 PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:untitled-game2>)
add_custom_command(TARGET untitled-game2 PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scripts/ $<TARGET_FILE_DIR:untitled-game2>)

# Build documentation with Doxygen.
# Only build documentation for release build.
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
	option(BUILD_DOC "Build documentation" ON)
	# check if doxygen is installed
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
	    # set input and output files
	    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
	    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	    # request to configure the file
	    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	    message(STATUS "Doxygen build started")
	    # NOTE: ALL builds doc together with app - all targets
	    add_custom_target(doc_doxygen ALL
	        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	        COMMENT "Generating API documentation with Doxygen"
	        VERBATIM)
	else (DOXYGEN_FOUND)
	    message(STATUS "Doxygen needs to be installed to generate doxygen documentation")
	endif (DOXYGEN_FOUND)
endif()
