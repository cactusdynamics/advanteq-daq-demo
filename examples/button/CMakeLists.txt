add_executable(advantech-demo-button
  main.cc
)

target_link_libraries(advantech-demo-button
  PRIVATE
  biodaq
)

target_compile_features(advantech-demo-button
  PUBLIC 
  c_std_11
  cxx_std_20
)