project(evoid)

add_executable(evoid 
# HEADERS
  include/menu.hpp

# SOURCES
  src/menu.cpp
  src/evoid.cpp
)

target_include_directories(evoid PRIVATE
  ./include
)

target_link_libraries(evoid PRIVATE
  draw
  $<$<PLATFORM_ID:Linux>:-rdynamic> # this flag needed backtrace, if need verbose stack trace
)
