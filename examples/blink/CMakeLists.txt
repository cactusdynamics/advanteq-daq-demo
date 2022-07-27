add_executable(advantech-demo-blink
  main.cc
)

target_link_libraries(advantech-demo-blink
  PRIVATE
  rt_app_lib
  biodaq
)
