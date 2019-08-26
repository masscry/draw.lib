cmake_minimum_required(VERSION 3.10)

project(draw
  VERSION 0.0.1.1 
  DESCRIPTION "New Graphics Engine"
  LANGUAGES CXX
)

find_program(CLANG_TIDY_CHECK NAMES clang-tidy)

if (CLANG_TIDY_CHECK)
  message(STATUS "clang-tidy checks enabled")
  set(CMAKE_CXX_CLANG_TIDY
    clang-tidy
    -checks=bugprone-*,cert-*,clang-*,modernize-*,perfomance-*,readability-*,misc-*,google-*,portability-*
    -header-filter=*
  )
endif(CLANG_TIDY_CHECK)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.2 REQUIRED)

add_subdirectory(contrib)

set(DRAW_VER_STRING "${draw_VERSION}")
set(DRAW_VER_MAJOR   ${draw_VERSION_MAJOR})
set(DRAW_VER_MINOR   ${draw_VERSION_MINOR})
set(DRAW_VER_PATCH   ${draw_VERSION_PATCH})
set(DRAW_VER_BUILD   ${draw_VERSION_TWEAK})

if(UNIX)
  set(DRAW_PLATFORM_UNIX 1)
else(UNIX)
  set(DRAW_PLATFORM_UNIX 0)
endif(UNIX)

if(WIN32)
  set(DRAW_PLATFORM_WIN32 1)
else(WIN32)
  set(DRAW_PLATFORM_WIN32 0)
endif(WIN32)

configure_file(include/draw/config.h.in include/draw/config.h @ONLY)

add_library(drawSetup INTERFACE)

target_compile_features(drawSetup INTERFACE cxx_std_11)

target_include_directories(drawSetup INTERFACE
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_compile_options(drawSetup INTERFACE -Wall)

add_library(draw 
  # SOURCES
  src/system_t.cpp
  src/shader_t.cpp
  src/vao_t.cpp
  src/settings_t.cpp

  # HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/include/draw/config.h
  include/draw.hpp
  include/draw/error_t.hpp
  include/draw/frameStage_t.hpp
  include/draw/shader_t.hpp
  include/draw/system_t.hpp
  include/draw/vao_t.hpp
  include/draw/settings_t.hpp
)

target_link_libraries(draw PUBLIC drawSetup glfw glad alpha0)

add_executable(ex_code_000
  examples/000.cpp
)

target_link_libraries(ex_code_000 PRIVATE draw alpha0)
