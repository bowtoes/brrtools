#ifndef BRRMACRO_MAP_H
#define BRRMACRO_MAP_H

#define _brr_map_eval0(...) __VA_ARGS__
#define _brr_map_eval1(...) _brr_map_eval0(_brr_map_eval0(_brr_map_eval0(__VA_ARGS__)))
#define _brr_map_eval2(...) _brr_map_eval1(_brr_map_eval1(_brr_map_eval1(__VA_ARGS__)))
#define _brr_map_eval3(...) _brr_map_eval2(_brr_map_eval2(_brr_map_eval2(__VA_ARGS__)))
#define _brr_map_eval4(...) _brr_map_eval3(_brr_map_eval3(_brr_map_eval3(__VA_ARGS__)))
#define _brr_map_eval(...)  _brr_map_eval4(_brr_map_eval4(_brr_map_eval4(__VA_ARGS__)))

/* https://github.com/swansontec/map-macro */
#define _brr_map_end(...)
#define _brr_map_out
#define _brr_map_comma ,

#define _brr_map_get_end2() 0, _brr_map_end
#define _brr_map_get_end1(...) _brr_map_get_end2
#define _brr_map_get_end(...) _brr_map_get_end1
#define _brr_map_next0(test, next, ...) next _brr_map_out
#define _brr_map_next1(test, next) _brr_map_next0(test, next, 0)
#define _brr_map_next(test, next)  _brr_map_next1(_brr_map_get_end test, next)

#define _brr_map0(f, x, peek, ...) f(x) _brr_map_next(peek, _brr_map1)(f, peek, __VA_ARGS__)
#define _brr_map1(f, x, peek, ...) f(x) _brr_map_next(peek, _brr_map0)(f, peek, __VA_ARGS__)

/* Applies the function macro 'f' to each of the remaining parameters. */
#define brr_map(f, ...) _brr_map_eval(_brr_map1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define _brr_map_list_next1(test, next) _brr_map_next0(test, _brr_map_comma next, 0)
#define _brr_map_list_next(test, next)  _brr_map_list_next1(_brr_map_get_end test, next)

#define _brr_map_list0(f, x, peek, ...) f(x) _brr_map_list_next(peek, _brr_map_list1)(f, peek, __VA_ARGS__)
#define _brr_map_list1(f, x, peek, ...) f(x) _brr_map_list_next(peek, _brr_map_list0)(f, peek, __VA_ARGS__)

/* Applies the function macro 'f' to each of the remaining parameters and inserts commas between the results.
 * */
#define brr_map_list(f, ...) _brr_map_eval(_brr_map_list1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif /* BRRMACRO_MAP_H */
