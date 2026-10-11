# orderbook

Minimal TCP order book server skeleton, with GoogleTest unit + latency tests and sanitizer builds.

Compiled with `-Wall -Wextra`. GoogleTest is downloaded automatically by CMake on first configure.

## Build & run the server

    cmake -B build
    cmake --build build
    ./build/orderbook          # port 9000
    ./build/orderbook 9001     # custom port

Stop with Ctrl+C.

Talk to it from another terminal:

    nc localhost 9000
    BUY 10 100.5
    SELL 5 101
    CANCEL 1
    BOOK

## Run tests

    cmake --build build
    ctest --test-dir build --output-on-failure

Or run the test binary directly (shows latency numbers):

    ./build/orderbook_tests

Only the latency tests:

    ./build/orderbook_tests --gtest_filter='Latency.*'

Include disabled tests (prefixed `DISABLED_`):

    ./build/orderbook_tests --gtest_also_run_disabled_tests

## Release build (real latency numbers)

Debug/sanitizer timings are much slower; use this when you care about the numbers.

    cmake -B build-release -DCMAKE_BUILD_TYPE=Release
    cmake --build build-release
    ./build-release/orderbook_tests --gtest_filter='Latency.*'

## Sanitizers

AddressSanitizer + UndefinedBehaviorSanitizer (memory bugs, UB):

    cmake -B build-asan -DSANITIZER=address
    cmake --build build-asan
    ctest --test-dir build-asan --output-on-failure

ThreadSanitizer (data races, e.g. in the server's client threads):

    cmake -B build-tsan -DSANITIZER=thread
    cmake --build build-tsan
    ctest --test-dir build-tsan --output-on-failure

The server can also be run under a sanitizer build, e.g. `./build-tsan/orderbook`.

## Layout

    src/OrderBook.*   order book logic (your part — TODO stubs)
    src/Server.*      TCP server, line-based commands
    src/main.cpp      entry point, Ctrl+C handling
    tests/            GoogleTest unit + latency tests
