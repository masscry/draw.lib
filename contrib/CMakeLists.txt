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

