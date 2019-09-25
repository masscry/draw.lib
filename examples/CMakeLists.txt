add_executable(ex000
# HEADERS
  ex000/console.hpp

# SOURCES
  ex000/ex000.cpp
  ex000/console.cpp
)

target_link_libraries(ex000 PRIVATE
  draw
  $<$<PLATFORM_ID:Linux>:-rdynamic> # this flag needed backtrace, if need verbose stack trace
)
