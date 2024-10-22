# cmake tested versions
cmake_minimum_required(VERSION 3.22...3.27)

# project name, description, & lang
project(speech-to-text
    DESCRIPTION "It's a voice"
    LANGUAGES CXX)
# set version numbers
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# declare exe
add_library(stt)

target_sources(stt PRIVATE src/speech-to-text.cpp)

target_include_directories(stt PRIVATE
    "${CMAKE_SOURCE_DIR}/include/stt")

target_include_directories(stt PRIVATE
    "../dep/linux/SFML-2.6.1/include/")
target_link_directories(stt PRIVATE
    "../dep/linux/SFML-2.6.1/lib/")
set(SFML_DIR "../dep/linux/SFML-2.6.1/lib/cmake/SFML")


target_include_directories(stt PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/deepspeech/")
target_link_directories(stt PRIVATE
    "${CMAKE_SOURCE_DIR}/lib/deepspeech/")


find_package(SFML COMPONENTS network graphics window system REQUIRED)

target_link_libraries(stt PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    OpenGL
    GL
    deepspeech
    )

# target include/.h
#target_include_directories(voice PRIVATE ${CMAKE_SOURCE_DIR}/include
#    dep/DeepSpeech/native_client
#    dep/DeepSpeech/native_client/kenlm
#    dep/DeepSpeech/native_client/kenlm/lm
#    dep/DeepSpeech/native_client/ctcdecode
#    dep/DeepSpeech/native_client/ctcdecode/third_party/object_pool
#    dep/DeepSpeech/native_client/ctcdecode/third_party/openfst-1.6.7/src/include
#    )

# target features & options
target_compile_features(stt PRIVATE cxx_std_20)
# m_contextdebug and release g++ flags
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

## DOXYGEN

#option(BUILD_DOC "Build documentation" ON)
# check if doxygen is installed
#find_package(Doxygen)
#if (DOXYGEN_FOUND)
    # set input and output files
    #    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    #    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    # request to configure the file
    #    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    #    message(STATUS "Doxygen build started")
    # NOTE: ALL builds doc together with app - all targets
    #    add_custom_target(doc_doxygen ALL
    #        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    #        COMMENT "Generating API documentation with Doxygen"
    #       VERBATIM)
    #else (DOXYGEN_FOUND)
    #message(STATUS "Doxygen needs to be installed to generate doxygen documentation")
    #endif (DOXYGEN_FOUND)
