cmake_minimum_required(VERSION 3.16)
project(orderbook CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# ---- Options ----
option(BUILD_TESTS "Build GoogleTest tests" ON)
set(SANITIZER "" CACHE STRING "Sanitizer to enable: address, thread, or empty for none")

# Warnings for everything we compile
add_compile_options(-Wall -Wextra)

if(SANITIZER STREQUAL "address")
  # AddressSanitizer + UndefinedBehaviorSanitizer
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
elseif(SANITIZER STREQUAL "thread")
  add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
  add_link_options(-fsanitize=thread)
elseif(NOT SANITIZER STREQUAL "")
  message(FATAL_ERROR "Unknown SANITIZER '${SANITIZER}' (use address or thread)")
endif()

# ---- Core library (shared by server and tests) ----
add_library(orderbook_lib
  src/OrderBook.cpp
  src/Server.cpp
)
target_include_directories(orderbook_lib PUBLIC src)

# ---- Server executable ----
add_executable(orderbook src/main.cpp)
target_link_libraries(orderbook PRIVATE orderbook_lib)

# ---- Tests ----
if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  FetchContent_MakeAvailable(googletest)

  enable_testing()
  include(GoogleTest)

  add_executable(orderbook_tests
    tests/OrderBookTest.cpp
    tests/LatencyTest.cpp
  )
  target_link_libraries(orderbook_tests PRIVATE orderbook_lib GTest::gtest_main)
  gtest_discover_tests(orderbook_tests)
endif()
