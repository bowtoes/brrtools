#!/usr/bin/env sh

make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=UNIX    MODE=SHARED BITS=32 again check-again
make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=UNIX    MODE=SHARED BITS=64 again check-again
make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=UNIX    MODE=STATIC BITS=32 again check-again
make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=UNIX    MODE=STATIC BITS=64 again check-again
make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=WINDOWS MODE=SHARED BITS=32 again check-again
make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=WINDOWS MODE=SHARED BITS=64 again check-again
make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=WINDOWS MODE=STATIC BITS=32 again check-again
make DEBUG=1 MEMCHECK=1 HOST=UNIX TARGET=WINDOWS MODE=STATIC BITS=64 again check-again
