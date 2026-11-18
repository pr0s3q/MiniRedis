# MiniRedis

A small in-memory key-value server, in the spirit of Redis, written in C++.

## Now

A TCP server on `127.0.0.1:60000`. Each connection is handled on its own thread: it reads one message, prints it, replies `OK`, and closes.

There is no command parser and no storage yet.

## Build

```sh
cmake -B build
cmake --build build
./build/MiniRedis
```

Requires CMake 4.4 and C++26.

## Clang

O1 with address sanitizer
```sh
clang++ -O1 -march=native -fsanitize=undefined,address \
  -Wall -Wextra -Wpedantic -Werror -Wshadow \
  -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-sanitize-recover=all \
  -std=c++26 \
  -DLOGS \
  -I./include ./src/*.cpp main.cpp \
  -o mini_redis_asan_o1
```

O1 with thread sanitizer
```sh
clang++ -O1 -march=native -fsanitize=undefined,thread \
  -Wall -Wextra -Wpedantic -Werror -Wshadow \
  -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-sanitize-recover=all \
  -std=c++26 \
  -DLOGS \
  -I./include ./src/*.cpp main.cpp \
  -o mini_redis_tsan_o1
```

O3 with address sanitizer
```sh
clang++ -O3 -march=native -fsanitize=undefined,address \
  -Wall -Wextra -Wpedantic -Werror -Wshadow \
  -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-sanitize-recover=all \
  -std=c++26 \
  -DLOGS \
  -I./include ./src/*.cpp main.cpp \
  -o mini_redis_asan_o3
```

O3 with thread sanitizer
```sh
clang++ -O3 -march=native -fsanitize=undefined,thread \
  -Wall -Wextra -Wpedantic -Werror -Wshadow \
  -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-sanitize-recover=all \
  -std=c++26 \
  -DLOGS \
  -I./include ./src/*.cpp main.cpp \
  -o mini_redis_tsan_o3
```

## Clang Tidy

```sh
clang-tidy \
  ./include/*.hpp ./src/*.cpp main.cpp \
  -checks='-*,
    bugprone-*,
    clang-analyzer-*,
    concurrency-*,
    cppcoreguidelines-*,
    performance-*,
    portability-*,
    readability-*,
    modernize-*,
    misc-include-cleaner,
    -portability-avoid-pragma-once,
    -modernize-use-trailing-return-type,
    -readability-braces-around-statements,
    -readability-function-cognitive-complexity,
    -readability-identifier-length' \
  -warnings-as-errors='*' \
  -header-filter='.*' \
  -- \
  -std=c++26 \
  -I./include
```