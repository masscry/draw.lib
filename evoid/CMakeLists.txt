project(evoid)

add_executable(evoid 
# HEADERS
  include/menu.hpp
  include/scenes.hpp

# SOURCES
  src/menu.cpp
  src/evoid.cpp
  src/scenes.cpp
)

target_include_directories(evoid PRIVATE
  ./include
)

target_link_libraries(evoid PRIVATE
  draw
  $<$<PLATFORM_ID:Linux>:-rdynamic> # this flag needed backtrace, if need verbose stack trace
)

set(EVOID_RUNTIME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/runtime/)

install(DIRECTORY ${EVOID_RUNTIME_DIR} DESTINATION . USE_SOURCE_PERMISSIONS)
install(TARGETS evoid DESTINATION bin)

