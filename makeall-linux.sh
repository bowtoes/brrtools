#!/usr/bin/env sh

make HOST=UNIX TARGET=UNIX    MODE=SHARED BITS=32 aio all check-aio check || exit
make HOST=UNIX TARGET=UNIX    MODE=SHARED BITS=64 aio all check-aio check || exit
make HOST=UNIX TARGET=UNIX    MODE=STATIC BITS=32 aio all check-aio check || exit
make HOST=UNIX TARGET=UNIX    MODE=STATIC BITS=64 aio all check-aio check || exit
make HOST=UNIX TARGET=WINDOWS MODE=SHARED BITS=32 aio all check-aio check || exit
make HOST=UNIX TARGET=WINDOWS MODE=SHARED BITS=64 aio all check-aio check || exit
make HOST=UNIX TARGET=WINDOWS MODE=STATIC BITS=32 aio all check-aio check || exit
make HOST=UNIX TARGET=WINDOWS MODE=STATIC BITS=64 aio all check-aio check || exit
