project(evoid)

add_executable(evoid 
  src/evoid.cpp
)

target_link_libraries(evoid PRIVATE
  draw
  $<$<PLATFORM_ID:Linux>:-rdynamic> # this flag needed backtrace, if need verbose stack trace
)
