add_executable(advantech-instantai
  main.cc
)

target_link_libraries(advantech-instantai
  PRIVATE
  biodaq
  fmt
)

target_compile_features(advantech-instantai
  PUBLIC
  c_std_11
  cxx_std_20
)
