/*
 * Copyright (C) 2012 William Swanson
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */

#ifndef BRRMAP_H
#define BRRMAP_H

/* https://github.com/swansontec/map-macro */
#define _brrmap_eval0(...) __VA_ARGS__
#define _brrmap_eval1(...) _brrmap_eval0(_brrmap_eval0(_brrmap_eval0(__VA_ARGS__)))
#define _brrmap_eval2(...) _brrmap_eval1(_brrmap_eval1(_brrmap_eval1(__VA_ARGS__)))
#define _brrmap_eval3(...) _brrmap_eval2(_brrmap_eval2(_brrmap_eval2(__VA_ARGS__)))
#define _brrmap_eval4(...) _brrmap_eval3(_brrmap_eval3(_brrmap_eval3(__VA_ARGS__)))
#define _brrmap_eval(...)  _brrmap_eval4(_brrmap_eval4(_brrmap_eval4(__VA_ARGS__)))

#define _brrmap_end(...)
#define _brrmap_out
#define _brrmap_comma ,

#define _brrmap_get_end2() 0, _brrmap_end
#define _brrmap_get_end1(...) _brrmap_get_end2
#define _brrmap_get_end(...) _brrmap_get_end1
#define _brrmap_next0(test, next, ...) next _brrmap_out
#define _brrmap_next1(test, next) _brrmap_next0(test, next, 0)
#define _brrmap_next(test, next)  _brrmap_next1(_brrmap_get_end test, next)

#define _brrmap0(f, x, peek, ...) f(x) _brrmap_next(peek, _brrmap1)(f, peek, __VA_ARGS__)
#define _brrmap1(f, x, peek, ...) f(x) _brrmap_next(peek, _brrmap0)(f, peek, __VA_ARGS__)

#define _brrmap_list_next1(test, next) _brrmap_next0(test, _brrmap_comma next, 0)
#define _brrmap_list_next(test, next)  _brrmap_list_next1(_brrmap_get_end test, next)

#define _brrmap_list0(f, x, peek, ...) f(x) _brrmap_list_next(peek, _brrmap_list1)(f, peek, __VA_ARGS__)
#define _brrmap_list1(f, x, peek, ...) f(x) _brrmap_list_next(peek, _brrmap_list0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro 'f' to each of the remaining parameters.
 */
#define BRRMAP(f, ...) _brrmap_eval(_brrmap1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/**
 * Applies the function macro 'f' to each of the remaining parameters and
 * inserts commas between the results.
 */
#define BRRMAP_LIST(f, ...) _brrmap_eval(_brrmap_list1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif /* BRRMAP_H */
