#ifndef BRRUTILS_H
#define BRRUTILS_H

#include <stdint.h>

// Is x a power of 2
#define IISP2(_x) ((_x)&&!((_x)&((_x)-1)))
// Is x a multiple of 4
#define IISM4(_x) (((_x)&3)==0)
// Give lowest int y such that 4|y and y>=x
#define IRND4(_x) (((_x)+3)&(~3))

#define MAGIC(_a,_b,_c,_d) (\
(u4)(\
    ((u4)((_a)<<0x00))|\
    ((u4)((_b)<<0x08))|\
    ((u4)((_c)<<0x10))|\
    ((u4)((_d)<<0x18))\
))
#define FOURCCU4(_u4) (\
(const char[4]){\
    ((char)(((_u4)>>0x18)&UINT64_C(0xFF))),\
    ((char)(((_u4)>>0x10)&UINT64_C(0xFF))),\
    ((char)(((_u4)>>0x08)&UINT64_C(0xFF))),\
    ((char)(((_u4)>>0x00)&UINT64_C(0xFF))),\
})
#define FOURCCU4R(_u4) (\
(const char[4]){\
    ((char)(((_u4)>>0x00)&UINT64_C(0xFF))),\
    ((char)(((_u4)>>0x08)&UINT64_C(0xFF))),\
    ((char)(((_u4)>>0x10)&UINT64_C(0xFF))),\
    ((char)(((_u4)>>0x18)&UINT64_C(0xFF))),\
})
#define FOURCC(_a,_b,_c,_d)  ((const char[4]){_a,_b,_c,_d})
#define FOURCCR(_a,_b,_c,_d) ((const char[4]){_d,_c,_b,_a})

#define TOGGLE(_a) ((_a)=!(_a))
#define HASFLAG(_x,_f) (((_x)&(_f))!=0)
#define HASFLAGS(_x,_f) (((_x)&(_f))==(_f))

#if defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
# define brr_read(_fd, _dst, _sz) _read(_fd, _dst, _sz)
# define brr_write(_fd, _src, _sz) _write(_fd, _src, _sz)
# define brr_lseek(_fd, _of, _whn) _lseek(_fd, _of, _whn)
# define brr_access(_pt, _md) _access_s(_pt, _md)
#else
# define brr_read(_fd, _dst, _sz) read(_fd, _dst, _sz)
# define brr_write(_fd, _src, _sz) write(_fd, _src, _sz)
# define brr_lseek(_fd, _of, _whn) lseek(_fd, _of, _whn)
# define brr_access(_pt, _md) access(_pt, _md)
#endif

#endif /* BRRUTILS_H */
