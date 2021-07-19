/*
Copyright 2021 BowToes (bow.toes@mailfence.com)

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

#ifndef BRRTIL_H
#define BRRTIL_H

#define _brrtil_eval0(...) __VA_ARGS__
#define _brrtil_eval1(...) _brrtil_eval0(_brrtil_eval0(_brrtil_eval0(__VA_ARGS__)))
#define _brrtil_eval2(...) _brrtil_eval1(_brrtil_eval1(_brrtil_eval1(__VA_ARGS__)))
#define _brrtil_eval3(...) _brrtil_eval2(_brrtil_eval2(_brrtil_eval2(__VA_ARGS__)))
#define _brrtil_eval4(...) _brrtil_eval3(_brrtil_eval3(_brrtil_eval3(__VA_ARGS__)))
#define _brrtil_eval(...)  _brrtil_eval4(_brrtil_eval4(_brrtil_eval4(__VA_ARGS__)))

#define _brrtil_narg0(...) _brrtil_narg1(__VA_ARGS__)
#define _brrtil_narg1( \
    _001_, _002_, _003_, _004_, _005_, _006_, _007_, _008_, _009_, _010_, \
    _011_, _012_, _013_, _014_, _015_, _016_, _017_, _018_, _019_, _020_, \
    _021_, _022_, _023_, _024_, _025_, _026_, _027_, _028_, _029_, _030_, \
    _031_, _032_, _033_, _034_, _035_, _036_, _037_, _038_, _039_, _040_, \
    _041_, _042_, _043_, _044_, _045_, _046_, _047_, _048_, _049_, _050_, \
    _051_, _052_, _053_, _054_, _055_, _056_, _057_, _058_, _059_, _060_, \
    _061_, _062_, _063_, _064_, _065_, _066_, _067_, _068_, _069_, _070_, \
    _071_, _072_, _073_, _074_, _075_, _076_, _077_, _078_, _079_, _080_, \
    _081_, _082_, _083_, _084_, _085_, _086_, _087_, _088_, _089_, _090_, \
    _091_, _092_, _093_, _094_, _095_, _096_, _097_, _098_, _099_, _100_, \
    _101_, _102_, _103_, _104_, _105_, _106_, _107_, _108_, _109_, _110_, \
    _111_, _112_, _113_, _114_, _115_, _116_, _117_, _118_, _119_, _120_, \
    _121_, _122_, _123_, _124_, _125_, _126_, _127_, _128_, _129_, _130_, \
    _131_, _132_, _133_, _134_, _135_, _136_, _137_, _138_, _139_, _140_, N, ...) N
#define _brrtil_sequence() \
    140, 139, 138, 137, 136, 135, 134, 133, 132, 131, \
    130, 129, 128, 127, 126, 125, 124, 123, 122, 121, \
    120, 119, 118, 117, 116, 115, 114, 113, 112, 111, \
    110, 109, 108, 107, 106, 105, 104, 103, 102, 101, \
    100, 99, 98, 97, 96, 95, 94, 93, 92, 91, \
    90, 89, 88, 87, 86, 85, 84, 83, 82, 81, \
    80, 79, 78, 77, 76, 75, 74, 73, 72, 71, \
    70, 69, 68, 67, 66, 65, 64, 63, 62, 61, \
    60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
    50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
    40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
    30, 29, 28, 27, 26, 25, 24, 23, 22, 21, \
    20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
    10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _brrtil_commas() \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0
#define _brrtil_comma(...) ,
#define _brrtil_has_comma(...) _brrtil_narg0(__VA_ARGS__, _brrtil_commas())

#define _brrtil_narg_help00(_n_) 1
#define _brrtil_narg_help01(_n_) 0
#define _brrtil_narg_help11(_n_) _n_
#define _brrtil_narg_help1(_a_, _b_, _n_) _brrtil_help##_a_##_b_(_n_)
#define _brrtil_narg_help0(_a_, _b_, _n_) _brrtil_narg_help1(_a_, _b_, _n_)

#define BRRTIL_NARG(...) \
    _brrtil_narg_help0( \
			_brrtil_has_comma(__VA_ARGS__), \
			_brrtil_has_comma(_brrtil_comma __VA_ARGS__ ()), \
			_brrtil_narg0(__VA_ARGS__, _brrtil_sequence()))

#define _brrtil_paste2(_1_, _2_) _1_ ## _2_
#define BRRTIL_PASTE(_1_,_2_) _brrtil_paste2(_1_, _2_)

#define _brrtil_index_get0(_0_, ...) (_0_)
#define _brrtil_index_after0(_0_, ...) __VA_ARGS__
#define BRRTIL_INDEX(_n_, ...) _brrtil_eval(BRRTIL_PASTE(_brrtil_index_get, _n_ (__VA_ARGS__)))
#define BRRTIL_AFTER(_n_, ...) _brrtil_eval(BRRTIL_PASTE(_brrtil_index_after, _n_ (__VA_ARGS__)))

#define BRRTIL_TOGGLE(_b_) ((_b_) = !(_b_))

#define BRRTIL_MIN(_1_, _2_) (_1_<_2_?_1_:_2_)
#define BRRTIL_MAX(_1_, _2_) (_1_>_2_?_1_:_2_)
#define BRRTIL_CLAMP(_v_, _m_, _M_) (_v_<_m_?_m_:_v_>_M_?_M_:_v_)

#define BRRTIL_ARRLEN(_a_) (sizeof(_a_)/sizeof((_a_)[0]))
#define BRRTIL_SAFESTR(_s_,_r_) ((_s_)?(_s_):(_r_))
#define BRRTIL_NULSTR(_s_) BRRTIL_SAFESTR(_s_,"(nul)")
#define BRRTIL_EMPTYSTR(_s_) BRRTIL_SAFESTR(_s_,"")
#define BRRTIL_ITERARR(_t_, _i_, _a_) for (_t_ _i_ = 0; _i_ < BRRTIL_ARRLEN((_a_)); ++_i_)
#define BRRTIL_ASARRAY(_t_, ...) ((_t_ []){__VA_ARGS__})

#define BRRTIL_ANYFLAG(_x_,_f_) (((_x_)&(_f_))!=0)
#define BRRTIL_ALLFLAGS(_x_,_f_) (((_x_)&(_f_))==(_f_))
#define BRRTIL_TGL(_a_) ((_a_)=!(_a_))

#if defined(BRRPLATFORMTYPE_WINDOWS)
# define BRRTIL_WIN_ULRG(_l_, _h_) ((ULARGE_INTEGER){.u={.LowPart=_l_, .HighPart=_h_}})
#endif

#endif /* BRRTIL_H */
