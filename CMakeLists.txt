cmake_minimum_required(VERSION 3.13)
project(rt_demo)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_CLANG_TIDY "Run clang-tidy" OFF)

add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Wnon-virtual-dtor
  -Wpedantic
  -Wold-style-cast
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wsign-conversion
  -Wmisleading-indentation
  -Wduplicated-cond
  -Wduplicated-branches
  -Wnull-dereference
  -Wuseless-cast
  -Wdouble-promotion
)

if (ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY clang-tidy clang-tidy-14 clang-tidy-13 clang-tidy-12 clang-tidy-11)
  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
else()
  message(STATUS "Not running clang-tidy. Use ENABLE_CLANG_TIDY=ON to run clang-tidy.")
endif()

include(FetchContent)

# Disable examples from rt-demo
set(ENABLE_EXAMPLES OFF)
FetchContent_Declare(rt-demo
  GIT_REPOSITORY https://github.com/cactusdynamics/rt-demo.git
  GIT_TAG master # FIXME: use a tagged release
)

FetchContent_MakeAvailable(rt-demo)

add_definitions(-DSPDLOG_ACTIVE_LEVEL=1) # debug level

find_package(spdlog REQUIRED)
find_package(DAQNavi REQUIRED)

add_subdirectory(examples/blink)
