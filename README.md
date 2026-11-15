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
