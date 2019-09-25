configure_file(include/draw/config.h.in include/draw/config.h @ONLY)

add_library(drawSetup INTERFACE)

target_compile_features(drawSetup INTERFACE cxx_std_11)

target_include_directories(drawSetup INTERFACE
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_compile_options(drawSetup INTERFACE -Wall)

add_library(draw STATIC
  # SOURCES
  src/system_t.cpp
  src/shader_t.cpp
  src/texture_t.cpp
  src/vao_t.cpp
  src/settings_t.cpp
  src/camera_t.cpp
  src/mesh_t.cpp
  src/wavefront.cpp
  src/targa.cpp
  src/input_t.cpp
  src/event_t.cpp

  # HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/include/draw/config.h
  include/draw.hpp
  include/draw/error_t.hpp
  include/draw/frameStage_t.hpp
  include/draw/shader_t.hpp
  include/draw/texture_t.hpp
  include/draw/system_t.hpp
  include/draw/vao_t.hpp
  include/draw/settings_t.hpp
  include/draw/camera_t.hpp
  include/draw/mesh_t.hpp
  include/draw/input_t.hpp
  include/draw/event_t.hpp
)

target_link_libraries(draw PUBLIC drawSetup glfw glad alpha0 glm_static)
