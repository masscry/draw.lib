cmake_minimum_required(VERSION 3.10)

project(contrib)

add_library(glad
  glad/src/glad.c
  glad/include/glad/glad.h
  glad/include/KHR/khrplatform.h
)

target_include_directories(glad PUBLIC
  glad/include
)

add_subdirectory(alpha0)

set(GLM_TEST_ENABLE_CXX_11 ON CACHE BOOL "Enable C++11" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "Disable Tests" FORCE)
set(GLM_TEST_ENABLE_SIMD_SSE2 ON CACHE BOOL "Enable SSE2" FORCE)

add_subdirectory(glm)
