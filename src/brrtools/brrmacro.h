/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrtools_brrmacro_h
#define brrtools_brrmacro_h

#ifdef __cplusplus
extern "C" {
#endif
/* Repeats macro '_X_' n times, passing the current call index as the first argument */
#define brr_repeat1(  _i_, _X_, ...) _X_(_i_ + 0, __VA_ARGS__)
#define brr_repeat2(  _i_, _X_, ...) brr_repeat1(  _i_ + 0, _X_, __VA_ARGS__) brr_repeat1(  _i_ + 1,   _X_, __VA_ARGS__)
#define brr_repeat4(  _i_, _X_, ...) brr_repeat2(  _i_ + 0, _X_, __VA_ARGS__) brr_repeat2(  _i_ + 2,   _X_, __VA_ARGS__)
#define brr_repeat8(  _i_, _X_, ...) brr_repeat4(  _i_ + 0, _X_, __VA_ARGS__) brr_repeat4(  _i_ + 4,   _X_, __VA_ARGS__)
#define brr_repeat16( _i_, _X_, ...) brr_repeat8(  _i_ + 0, _X_, __VA_ARGS__) brr_repeat8(  _i_ + 8,   _X_, __VA_ARGS__)
#define brr_repeat32( _i_, _X_, ...) brr_repeat16( _i_ + 0, _X_, __VA_ARGS__) brr_repeat16( _i_ + 16,  _X_, __VA_ARGS__)
#define brr_repeat64( _i_, _X_, ...) brr_repeat32( _i_ + 0, _X_, __VA_ARGS__) brr_repeat32( _i_ + 32,  _X_, __VA_ARGS__)
#define brr_repeat128(_i_, _X_, ...) brr_repeat64( _i_ + 0, _X_, __VA_ARGS__) brr_repeat64( _i_ + 64,  _X_, __VA_ARGS__)
#define brr_repeat256(_i_, _X_, ...) brr_repeat128(_i_ + 0, _X_, __VA_ARGS__) brr_repeat128(_i_ + 128, _X_, __VA_ARGS__)
#define brr_repeat512(_i_, _X_, ...) brr_repeat256(_i_ + 0, _X_, __VA_ARGS__) brr_repeat256(_i_ + 256, _X_, __VA_ARGS__)

#define brr_crepeat2(  _i_, _X_, ...) brr_repeat1(  _i_ + 0, _X_, __VA_ARGS__), brr_repeat1(  _i_ + 1,   _X_, __VA_ARGS__)
#define brr_crepeat4(  _i_, _X_, ...) brr_crepeat2(  _i_ + 0, _X_, __VA_ARGS__), brr_crepeat2(  _i_ + 2,   _X_, __VA_ARGS__)
#define brr_crepeat8(  _i_, _X_, ...) brr_crepeat4(  _i_ + 0, _X_, __VA_ARGS__), brr_crepeat4(  _i_ + 4,   _X_, __VA_ARGS__)
#define brr_crepeat16( _i_, _X_, ...) brr_crepeat8(  _i_ + 0, _X_, __VA_ARGS__), brr_crepeat8(  _i_ + 8,   _X_, __VA_ARGS__)
#define brr_crepeat32( _i_, _X_, ...) brr_crepeat16( _i_ + 0, _X_, __VA_ARGS__), brr_crepeat16( _i_ + 16,  _X_, __VA_ARGS__)
#define brr_crepeat64( _i_, _X_, ...) brr_crepeat32( _i_ + 0, _X_, __VA_ARGS__), brr_crepeat32( _i_ + 32,  _X_, __VA_ARGS__)
#define brr_crepeat128(_i_, _X_, ...) brr_crepeat64( _i_ + 0, _X_, __VA_ARGS__), brr_crepeat64( _i_ + 64,  _X_, __VA_ARGS__)
#define brr_crepeat256(_i_, _X_, ...) brr_crepeat128(_i_ + 0, _X_, __VA_ARGS__), brr_crepeat128(_i_ + 128, _X_, __VA_ARGS__)
#define brr_crepeat512(_i_, _X_, ...) brr_crepeat256(_i_ + 0, _X_, __VA_ARGS__), brr_crepeat256(_i_ + 256, _X_, __VA_ARGS__)

#define _brr_eval0(...) __VA_ARGS__
#define _brr_eval1(...) _brr_eval0(_brr_eval0(_brr_eval0(_brr_eval0(__VA_ARGS__))))
#define _brr_eval2(...) _brr_eval1(_brr_eval1(_brr_eval1(_brr_eval1(__VA_ARGS__))))
#define _brr_eval3(...) _brr_eval2(_brr_eval2(_brr_eval2(_brr_eval2(__VA_ARGS__))))
#define _brr_eval4(...) _brr_eval3(_brr_eval3(_brr_eval3(_brr_eval3(__VA_ARGS__))))
#define _brr_eval5(...) _brr_eval4(_brr_eval4(_brr_eval4(_brr_eval4(__VA_ARGS__))))
#define brr_eval(...) _brr_eval5(__VA_ARGS__)

#define brr_stringify(_x_) #_x_
#define brr_estringify(_x_) brr_stringify(brr_eval(_x_))

/* https://github.com/swansontec/map-macro */
#define _brr_narg1( \
	      _01_, _02_, _03_, _04_, _05_, _06_, _07_, _08_, _09_, _0A_, _0B_, _0C_, _0D_, _0E_, _0F_, \
	_10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _1A_, _1B_, _1C_, _1D_, _1E_, _1F_, \
	_20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _2A_, _2B_, _2C_, _2D_, _2E_, _2F_, \
	_30_, _31_, _32_, _33_, _34_, _35_, _36_, _37_, _38_, _39_, _3A_, _3B_, _3C_, _3D_, _3E_, _3F_, \
	_40_, _41_, _42_, _43_, _44_, _45_, _46_, _47_, _48_, _49_, _4A_, _4B_, _4C_, _4D_, _4E_, _4F_, \
	_50_, _51_, _52_, _53_, _54_, _55_, _56_, _57_, _58_, _59_, _5A_, _5B_, _5C_, _5D_, _5E_, _5F_, \
	_60_, _61_, _62_, _63_, _64_, _65_, _66_, _67_, _68_, _69_, _6A_, _6B_, _6C_, _6D_, _6E_, _6F_, \
	_70_, _71_, _72_, _73_, _74_, _75_, _76_, _77_, _78_, _79_, _7A_, _7B_, _7C_, _7D_, _7E_, _7F_, \
	_80_, _81_, _82_, _83_, _84_, _85_, _86_, _87_, _88_, _89_, _8A_, _8B_, _8C_, _8D_, _8E_, _8F_, \
	_90_, _91_, _92_, _93_, _94_, _95_, _96_, _97_, _98_, _99_, _9A_, _9B_, _9C_, _9D_, _9E_, _9F_, \
	_A0_, _A1_, _A2_, _A3_, _A4_, _A5_, _A6_, _A7_, _A8_, _A9_, _AA_, _AB_, _AC_, _AD_, _AE_, _AF_, \
	_B0_, _B1_, _B2_, _B3_, _B4_, _B5_, _B6_, _B7_, _B8_, _B9_, _BA_, _BB_, _BC_, _BD_, _BE_, _BF_, \
	_C0_, _C1_, _C2_, _C3_, _C4_, _C5_, _C6_, _C7_, _C8_, _C9_, _CA_, _CB_, _CC_, _CD_, _CE_, _CF_, \
	_D0_, _D1_, _D2_, _D3_, _D4_, _D5_, _D6_, _D7_, _D8_, _D9_, _DA_, _DB_, _DC_, _DD_, _DE_, _DF_, \
	_E0_, _E1_, _E2_, _E3_, _E4_, _E5_, _E6_, _E7_, _E8_, _E9_, _EA_, _EB_, _EC_, _ED_, _EE_, _EF_, \
	_F0_, _F1_, _F2_, _F3_, _F4_, _F5_, _F6_, _F7_, _F8_, _F9_, _FA_, _FB_, _FC_, _FD_, _FE_, _FF_, \
	_NN_, ...) _NN_
#define _brr_narg0(...) _brr_narg1(__VA_ARGS__)
#define _brr_sequence() \
	255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, \
	239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, \
	223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, \
	207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192, \
	191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, \
	175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160, \
	159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, \
	143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, \
	127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, \
	111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100,  99,  98,  97,  96, \
	 95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80, \
	 79,  78,  77,  76,  75,  74,  73,  72,  71,  70,  69,  68,  67,  66,  65,  64, \
	 63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48, \
	 47,  46,  45,  44,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  32, \
	 31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16, \
	 15,  14,  13,  12,  11,  10,   9,   8,   7,   6,   5,   4,   3,   2,   1,   0
#define _brr_commas() \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0
#define _brr_comma(...) ,
#define _brr_has_comma(...) _brr_narg0(__VA_ARGS__, _brr_commas())

#define _brr_narg_help00(_n_) 1
#define _brr_narg_help01(_n_) 0
#define _brr_narg_help11(_n_) _n_
#define _brr_narg_help1(_a_, _b_, _n_) _brr_narg_help##_a_##_b_(_n_)
#define _brr_narg_help0(_a_, _b_, _n_) _brr_narg_help1(_a_, _b_, _n_)

#define brr_narg(...) \
	_brr_narg_help0( \
		_brr_has_comma(__VA_ARGS__), \
		_brr_has_comma(_brr_comma __VA_ARGS__ ()), \
		_brr_narg0(__VA_ARGS__, _brr_sequence()))

#define _brrjoin2(_1_, _2_) _1_ ## _2_
#define brr_join(_1_,_2_) _brrjoin2(_1_, _2_)
#define brr_ejoin(_1_,_2_) brr_join(brr_eval(_1_), brr_eval(_2_))

#define brr_array_length(_a_) (sizeof(_a_)/sizeof(*(_a_)))
#define brr_array_iter(_t_, _i_, _a_) for (_t_ _i_ = 0; _i_ < brrarray_length(_a_); ++_i_)
#define brr_array(_t_, ...) ((_t_ []){__VA_ARGS__})

#define brr_any(_x_, _f_) (((_x_)&(_f_))!=0)
#define brr_all(_x_, _f_) (((_x_)&(_f_))==(_f_))
#define brr_none(_x_, _f_) (((_x_)&(_f_))==0)

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

/* Applies the macro 'X' to each of the following arguments. */
#define brr_map(X, ...) _brr_map_eval(_brr_map1(X, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define _brr_map_list_next1(test, next) _brr_map_next0(test, _brr_map_comma next, 0)
#define _brr_map_list_next(test, next)  _brr_map_list_next1(_brr_map_get_end test, next)

#define _brr_map_list0(f, x, peek, ...) f(x) _brr_map_list_next(peek, _brr_map_list1)(f, peek, __VA_ARGS__)
#define _brr_map_list1(f, x, peek, ...) f(x) _brr_map_list_next(peek, _brr_map_list0)(f, peek, __VA_ARGS__)


/* Applies the macro 'X' to each of the following arguments and inserts commas between the output.
 * */
#define brr_map_list(X, ...) _brr_map_eval(_brr_map_list1(X, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define brr_bit(_i_, _mask_) (((_i_) & (_mask_)) != 0)
#define brr_bits(_i_, _mask_) (((_i_) & (_mask_)) == (_mask_))

#define brr_bit_mask_set(_i_, _mask_, _value_) ((_i_) = ((_i_) & ~(_mask_)) | ((_value_) & (_mask_)))
#define brr_bit_mask(_i_, _mask_) ((_i_) & (_mask_))

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrmacro_h */
