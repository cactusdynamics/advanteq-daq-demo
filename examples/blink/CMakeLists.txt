add_executable(advantech-demo-blink
  main.cc
)

target_link_libraries(advantech-demo-blink
  PRIVATE
  biodaq
  fmt
)

target_compile_features(advantech-demo-blink
  PUBLIC
  c_std_11
  cxx_std_20
)
