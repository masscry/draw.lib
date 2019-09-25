cmake_minimum_required(VERSION 3.10)

project(draw
  VERSION 0.0.1.1 
  DESCRIPTION "New Graphics Engine"
  LANGUAGES CXX
)

include(CPack)

if (DRAW_USE_CLANG_TIDY)
  find_program(CLANG_TIDY_CHECK NAMES clang-tidy)

  if (CLANG_TIDY_CHECK)
    message(STATUS "clang-tidy checks enabled")
    set(CMAKE_CXX_CLANG_TIDY
      clang-tidy
      -checks=bugprone-*,cert-*,clang-*,modernize-*,perfomance-*,readability-*,misc-*,google-*,portability-*
      -header-filter=*
    )
  endif(CLANG_TIDY_CHECK)
endif(DRAW_USE_CLANG_TIDY)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 3.2 REQUIRED)

add_subdirectory(contrib EXCLUDE_FROM_ALL)

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

add_subdirectory(engine)

add_subdirectory(examples EXCLUDE_FROM_ALL)

add_subdirectory(evoid)
