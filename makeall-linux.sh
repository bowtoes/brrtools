#!/usr/bin/env sh

make HOST=LINUX TARGET=UNIX    MODE=SHARED BITS=32 aio all check-aio check || exit
make HOST=LINUX TARGET=UNIX    MODE=SHARED BITS=64 aio all check-aio check || exit
make HOST=LINUX TARGET=UNIX    MODE=STATIC BITS=32 aio all check-aio check || exit
make HOST=LINUX TARGET=UNIX    MODE=STATIC BITS=64 aio all check-aio check || exit
make HOST=LINUX TARGET=WINDOWS MODE=SHARED BITS=32 aio all check-aio check || exit
make HOST=LINUX TARGET=WINDOWS MODE=SHARED BITS=64 aio all check-aio check || exit
make HOST=LINUX TARGET=WINDOWS MODE=STATIC BITS=32 aio all check-aio check || exit
make HOST=LINUX TARGET=WINDOWS MODE=STATIC BITS=64 aio all check-aio check || exit
