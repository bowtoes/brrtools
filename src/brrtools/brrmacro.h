/*
Copyright 2021-2022 BowToes (bow.toes@mailfence.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef BRRMACRO_H
#define BRRMACRO_H

#define _brrmacro_eval0(...) __VA_ARGS__
#define _brrmacro_eval1(...) _brrmacro_eval0(_brrmacro_eval0(_brrmacro_eval0(_brrmacro_eval0(__VA_ARGS__))))
#define _brrmacro_eval2(...) _brrmacro_eval1(_brrmacro_eval1(_brrmacro_eval1(_brrmacro_eval1(__VA_ARGS__))))
#define _brrmacro_eval3(...) _brrmacro_eval2(_brrmacro_eval2(_brrmacro_eval2(_brrmacro_eval2(__VA_ARGS__))))
#define _brrmacro_eval4(...) _brrmacro_eval3(_brrmacro_eval3(_brrmacro_eval3(_brrmacro_eval3(__VA_ARGS__))))
#define _brrmacro_eval5(...) _brrmacro_eval4(_brrmacro_eval4(_brrmacro_eval4(_brrmacro_eval4(__VA_ARGS__))))
#define BRRMACRO_EVAL(...) _brrmacro_eval5(__VA_ARGS__)

#define _brrmacro_narg1( \
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
#define _brrmacro_narg0(...) _brrmacro_narg1(__VA_ARGS__)
#define _brrmacro_sequence() \
	0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0, \
	0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1, 0xE0, \
	0xDF, 0xDE, 0xDD, 0xDC, 0xDB, 0xDA, 0xD9, 0xD8, 0xD7, 0xD6, 0xD5, 0xD4, 0xD3, 0xD2, 0xD1, 0xD0, \
	0xCF, 0xCE, 0xCD, 0xCC, 0xCB, 0xCA, 0xC9, 0xC8, 0xC7, 0xC6, 0xC5, 0xC4, 0xC3, 0xC2, 0xC1, 0xC0, \
	0xBF, 0xBE, 0xBD, 0xBC, 0xBB, 0xBA, 0xB9, 0xB8, 0xB7, 0xB6, 0xB5, 0xB4, 0xB3, 0xB2, 0xB1, 0xB0, \
	0xAF, 0xAE, 0xAD, 0xAC, 0xAB, 0xAA, 0xA9, 0xA8, 0xA7, 0xA6, 0xA5, 0xA4, 0xA3, 0xA2, 0xA1, 0xA0, \
	0x9F, 0x9E, 0x9D, 0x9C, 0x9B, 0x9A, 0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0x92, 0x91, 0x90, \
	0x8F, 0x8E, 0x8D, 0x8C, 0x8B, 0x8A, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80, \
	0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70, \
	0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60, \
	0x5F, 0x5E, 0x5D, 0x5C, 0x5B, 0x5A, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0x50, \
	0x4F, 0x4E, 0x4D, 0x4C, 0x4B, 0x4A, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40, \
	0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30, \
	0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, \
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, \
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
#define _brrmacro_commas() \
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
#define _brrmacro_comma(...) ,
#define _brrmacro_has_comma(...) _brrmacro_narg0(__VA_ARGS__, _brrmacro_commas())

#define _brrmacro_narg_help00(_n_) 1
#define _brrmacro_narg_help01(_n_) 0
#define _brrmacro_narg_help11(_n_) _n_
#define _brrmacro_narg_help1(_a_, _b_, _n_) _brrmacro_narg_help##_a_##_b_(_n_)
#define _brrmacro_narg_help0(_a_, _b_, _n_) _brrmacro_narg_help1(_a_, _b_, _n_)

#define BRRMACRO_NARG(...) \
	_brrmacro_narg_help0( \
		_brrmacro_has_comma(__VA_ARGS__), \
		_brrmacro_has_comma(_brrmacro_comma __VA_ARGS__ ()), \
		_brrmacro_narg0(__VA_ARGS__, _brrmacro_sequence()))

#define _brrmacro_paste2(_1_, _2_) _1_ ## _2_
#define BRRMACRO_PASTE(_1_,_2_) _brrmacro_paste2(_1_, _2_)

//#define _brrmacro_index_get0(_0_, ...) (_0_)
//#define _brrmacro_index_after0(_0_, ...) __VA_ARGS__
//#define BRRMACRO_INDEX(_n_, ...) _brrmacro_eval(BRRMACRO_PASTE(_brrmacro_index_get0, _n_ (__VA_ARGS__)))
//#define BRRMACRO_AFTER(_n_, ...) _brrmacro_eval(BRRMACRO_PASTE(_brrmacro_index_after0, _n_ (__VA_ARGS__)))

#define BRRMACRO_STRINGIFY(_x_) #_x_

#define BRR_ARRAY_LENGTH(_a_) (sizeof(_a_)/sizeof((_a_)[0]))
#define BRR_SAFESTR(_s_,_r_) ((_s_)?(_s_):(_r_))
#define BRR_NULL_STR(_s_)   BRR_SAFESTR(_s_, "(nul)")
#define BRR_EMPTY_STR(_s_) BRR_SAFESTR(_s_, "")
#define BRR_ITER_ARRAY(_t_, _i_, _a_) for (_t_ _i_ = 0; _i_ < BRR_ARRAY_LENGTH(_a_); ++_i_)
#define BRR_AS_ARRAY(_t_, ...) ((_t_ []){__VA_ARGS__})

#define BRR_MATCH_ANY(_x_, _f_) (((_x_)&(_f_))!=0)
#define BRR_MATCH_ALL(_x_, _f_) (((_x_)&(_f_))==(_f_))
#define BRR_TOGGLE(_a_) ((_a_)=!(_a_))

#define BRR_AS_CSTR(_x_) ((const char *)#_x_)

#endif /* BRRMACRO_H */
