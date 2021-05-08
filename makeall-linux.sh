#!/usr/bin/env sh

make HOST=LINUX TARGET=LINUX MODE=SHARED BITS=32 int ass obj all check-int check-ass check-obj check
make HOST=LINUX TARGET=LINUX MODE=SHARED BITS=64 int ass obj all check-int check-ass check-obj check
make HOST=LINUX TARGET=LINUX MODE=STATIC BITS=32 int ass obj all check-int check-ass check-obj check
make HOST=LINUX TARGET=LINUX MODE=STATIC BITS=64 int ass obj all check-int check-ass check-obj check
make HOST=LINUX TARGET=WINDOWS MODE=SHARED BITS=32 int ass obj all check-int check-ass check-obj check
make HOST=LINUX TARGET=WINDOWS MODE=SHARED BITS=64 int ass obj all check-int check-ass check-obj check
make HOST=LINUX TARGET=WINDOWS MODE=STATIC BITS=32 int ass obj all check-int check-ass check-obj check
make HOST=LINUX TARGET=WINDOWS MODE=STATIC BITS=64 int ass obj all check-int check-ass check-obj check
