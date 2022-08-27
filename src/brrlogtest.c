#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <brrtools/brrlog.h>

#if 1
#define _brrlog_try_pri(_lbl_, _pfx_, _dst_, _type_) do {\
	if (brrlog_priority_mod(_lbl_, \
		(brrlog_priority_t) {\
			.pfx=_pfx_,\
			.dst={\
				.dst=_dst_,\
				.type=_type_,\
			}\
		})) {\
		int e = brrapi_gete();\
		if (e == BRRAPI_E_LIBC) {\
			fprintf(stderr, "Failed to initilaize brrlog priority %i '%s': %s (%i)\n", _lbl_, _pfx_?_pfx_:"", strerror(errno), errno);\
		} else {\
			fprintf(stderr, "Failed to initilaize brrlog priority %i '%s': %s (%i)\n", _lbl_, _pfx_?_pfx_:"", brrapi_err(), brrapi_gete());\
		}\
		brrlog_deinit();\
		return 1;\
	}\
} while (0)

int
main(int argc, char **argv)
{
	if (brrlog_init(brrlog_default_cfg, "(!", "!)")) {
		fprintf(stderr, "Failed to init brrlog: %s (%i)\n", strerror(errno), errno);
		return 1;
	}
	_brrlog_try_pri(0, "(!f=rs=r:[WOAH]!): ", stdout, brrlog_dst_stream);
	_brrlog_try_pri(1, "(!f=gs=r:[WOAH]!): ", stdout, brrlog_dst_stream);
	_brrlog_try_pri(2, "(!f=bs=r:[WOAH]!): ", stdout, brrlog_dst_stream);
	_brrlog_try_pri(3, "(!f=Rs=r:[WAH]!): ", stdout, brrlog_dst_stream);

	brrlog(0, "Nice!");
	brrlog(1, "Nice!");
	brrlog(2, "Nice!");
	brrlog(3, "Nice!");

	brrlog_deinit();
	return 0;
}
#else
#include <stdlib.h>
#include <stdarg.h>

static inline void a(const char *const msg, ...)
{
	brrsz i = strlen(msg);
	va_list lptr;
	va_start(lptr, msg);
	brrsz mlen = brrlogv_text(0, (brrlog_source_t){0}, (brrlog_settings_t){.print_newline=1}, msg, lptr);
	va_end(lptr);
}

#define err(_msg_) fprintf(stderr, _msg_": %s (%i) (%s (%i))\n", brrapi_err(), brrapi_gete(), strerror(errno), errno)
int main(int argc, char **argv)
{
	if (brrlog_init(brrlog_default_cfg, "(!", "!)")) {
		err("Failed to init brrlog");
		return 1;
	}

#if 0
	brrlog("Nice");
	brrlog("AAAA (!f=r:%s!) AAAA", "NICE");
	brrlog("I AM NOT RED BUT '(!f=rs=b:%s!)' MOST (!s=if=g:%s!) IS!", "SOME STRING CUBED", "CERTAINLY");
#else
	brrlog(0, "(!f=0:fg=normal!)       (!b=0:bg=normal!)       (!s=0:st=normal!)       (normal)       (!_=0:fn=0!)");
	brrlog(0, "(!f=k:fg=black!)        (!b=k:bg=black!)        (!s=b:st=bold!)         (bold)         (!_=1:fn=1!)");
	brrlog(0, "(!f=r:fg=red!)          (!b=r:bg=red!)          (!s=d:st=dim!)          (dim)          (!_=2:fn=2!)");
	brrlog(0, "(!f=g:fg=green!)        (!b=g:bg=green!)        (!s=i:st=italics!)      (italics)      (!_=3:fn=3!)");
	brrlog(0, "(!f=y:fg=yellow!)       (!b=y:bg=yellow!)       (!s=u:st=under!)        (under)        (!_=4:fn=4!)");
	brrlog(0, "(!f=b:fg=blue!)         (!b=b:bg=blue!)         (!s=k:st=blink!)        (blink)        (!_=5:fn=5!)");
	brrlog(0, "(!f=m:fg=magenta!)      (!b=m:bg=magenta!)      (!s=t:st=fastblink!)    (fastblink)    (!_=6:fn=6!)");
	brrlog(0, "(!f=c:fg=cyan!)         (!b=c:bg=cyan!)         (!s=r:st=reverse!)      (reverse)      (!_=7:fn=7!)");
	brrlog(0, "(!f=w:fg=white!)        (!b=w:bg=white!)        (!s=c:st=conceal!)      (conceal)      (!_=8:fn=8!)");
	brrlog(0, "(!f=K:fg=grey!)         (!b=K:bg=grey!)         (!s=s:st=strikeout!)    (strikeout)    (!_=9:fn=9!)");
	brrlog(0, "(!f=R:fg=lightred!)     (!b=R:bg=lightred!)     (!s=f:st=fraktur!)      (fraktur)      (!_=-:fn=last!)");
	brrlog(0, "(!f=G:fg=lightgreen!)   (!b=G:bg=lightgreen!)   (!s=B:st=nobold!)       (nobold)       (!_=0:fn=0!)");
	brrlog(0, "(!f=Y:fg=lightyellow!)  (!b=Y:bg=lightyellow!)  (!s=G:st=nobright!)     (nobright)     (!_=1:fn=1!)");
	brrlog(0, "(!f=B:fg=lightblue!)    (!b=B:bg=lightblue!)    (!s=I:st=noitalics!)    (noitalics)    (!_=2:fn=2!)");
	brrlog(0, "(!f=M:fg=lightmagenta!) (!b=M:bg=lightmagenta!) (!s=U:st=nounder!)      (nounder)      (!_=3:fn=3!)");
	brrlog(0, "(!f=C:fg=lightcyan!)    (!b=C:bg=lightcyan!)    (!s=K:st=noblink!)      (noblink)      (!_=4:fn=4!)");
	brrlog(0, "(!f=-:fg=last!)         (!b=-:bg=last!)         (!s=v:st=reveal!)       (reveal)       (!_=6:fn=6!)");
	brrlog(0, "(!f=W:fg=lightwhite!)   (!b=W:bg=lightwhite!)   (!s=R:st=noreverse!)    (noreverse)    (!_=5:fn=5!)");
	brrlog(0, "(!f=0:fg=normal!)       (!b=0:bg=normal!)       (!s=S:st=nostrikeout!)  (nostrikeout)  (!_=7:fn=7!)");
	brrlog(0, "(!f=k:fg=black!)        (!b=k:bg=black!)        (!s=m:st=frame!)        (frame)        (!_=8:fn=8!)");
	brrlog(0, "(!f=r:fg=red!)          (!b=r:bg=red!)          (!s=l:st=circle!)       (circle)       (!_=9:fn=9!)");
	brrlog(0, "(!f=g:fg=green!)        (!b=g:bg=green!)        (!s=o:st=over!)         (over)         (!_=-:fn=last!)");
	brrlog(0, "(!f=y:fg=yellow!)       (!b=y:bg=yellow!)       (!s=M:st=noframe!)      (noframe)      (!_=0:fn=0!)");
	brrlog(0, "(!f=b:fg=blue!)         (!b=b:bg=blue!)         (!s=O:st=noover!)       (noover)       (!_=1:fn=1!)");
	brrlog(0, "(!f=m:fg=magenta!)      (!b=m:bg=magenta!)      (!s=V:st=iunder!)       (iunder)       (!_=2:fn=2!)");
	brrlog(0, "(!f=c:fg=cyan!)         (!b=c:bg=cyan!)         (!s=W:st=idoubleunder!) (idoubleunder) (!_=3:fn=3!)");
	brrlog(0, "(!f=w:fg=white!)        (!b=w:bg=white!)        (!s=X:st=iover!)        (iover)        (!_=4:fn=4!)");
	brrlog(0, "(!f=K:fg=grey!)         (!b=K:bg=grey!)         (!s=Y:st=idoubleover!)  (idoubleover)  (!_=5:fn=5!)");
	brrlog(0, "(!f=R:fg=lightred!)     (!b=R:bg=lightred!)     (!s=Z:st=istress!)      (istress)      (!_=6:fn=6!)");
	brrlog(0, "(!f=G:fg=lightgreen!)   (!b=G:bg=lightgreen!)   (!s=z:st=ioff!)         (ioff)         (!_=7:fn=7!)");
	brrlog(0, "(!f=-:fg=last!)         (!b=-:bg=last!)         (!s=-:st=last!)         (last)         (!_=8:fn=8!)");

	brrlog(0, "(!b=0f=0: fg=nrm bg=nrm !)(!b=-f=k: fg=blk bg=nrm !)(!b=-f=r: fg=red bg=nrm !)(!b=-f=g: fg=grn bg=nrm !)(!b=-f=y: fg=ylw bg=nrm !)(!b=-f=b: fg=blu bg=nrm !)(!b=-f=m: fg=mgn bg=nrm !)(!b=-f=c: fg=cyn bg=nrm !)(!b=-f=w: fg=wht bg=nrm !)(!b=-f=-K: fg=gry bg=nrm !)(!b=-f=R: fg=lrd bg=nrm !)(!b=-f=G: fg=lgr bg=nrm !)(!b=-f=Y: fg=lyw bg=nrm !)(!b=-f=B: fg=lbl bg=nrm !)(!b=-f=M: fg=lmg bg=nrm !)(!b=-f=C: fg=lcy bg=nrm !)(!b=-f=-: fg=lst bg=nrm !)");
	brrlog(0, "(!b=kf=0: fg=nrm bg=blk !)(!b=-f=k: fg=blk bg=blk !)(!b=-f=r: fg=red bg=blk !)(!b=-f=g: fg=grn bg=blk !)(!b=-f=y: fg=ylw bg=blk !)(!b=-f=b: fg=blu bg=blk !)(!b=-f=m: fg=mgn bg=blk !)(!b=-f=c: fg=cyn bg=blk !)(!b=-f=w: fg=wht bg=blk !)(!b=-f=-K: fg=gry bg=blk !)(!b=-f=R: fg=lrd bg=blk !)(!b=-f=G: fg=lgr bg=blk !)(!b=-f=Y: fg=lyw bg=blk !)(!b=-f=B: fg=lbl bg=blk !)(!b=-f=M: fg=lmg bg=blk !)(!b=-f=C: fg=lcy bg=blk !)(!b=-f=-: fg=lst bg=blk !)");
	brrlog(0, "(!b=rf=0: fg=nrm bg=red !)(!b=-f=k: fg=blk bg=red !)(!b=-f=r: fg=red bg=red !)(!b=-f=g: fg=grn bg=red !)(!b=-f=y: fg=ylw bg=red !)(!b=-f=b: fg=blu bg=red !)(!b=-f=m: fg=mgn bg=red !)(!b=-f=c: fg=cyn bg=red !)(!b=-f=w: fg=wht bg=red !)(!b=-f=-K: fg=gry bg=red !)(!b=-f=R: fg=lrd bg=red !)(!b=-f=G: fg=lgr bg=red !)(!b=-f=Y: fg=lyw bg=red !)(!b=-f=B: fg=lbl bg=red !)(!b=-f=M: fg=lmg bg=red !)(!b=-f=C: fg=lcy bg=red !)(!b=-f=-: fg=lst bg=red !)");
	brrlog(0, "(!b=gf=0: fg=nrm bg=grn !)(!b=-f=k: fg=blk bg=grn !)(!b=-f=r: fg=red bg=grn !)(!b=-f=g: fg=grn bg=grn !)(!b=-f=y: fg=ylw bg=grn !)(!b=-f=b: fg=blu bg=grn !)(!b=-f=m: fg=mgn bg=grn !)(!b=-f=c: fg=cyn bg=grn !)(!b=-f=w: fg=wht bg=grn !)(!b=-f=-K: fg=gry bg=grn !)(!b=-f=R: fg=lrd bg=grn !)(!b=-f=G: fg=lgr bg=grn !)(!b=-f=Y: fg=lyw bg=grn !)(!b=-f=B: fg=lbl bg=grn !)(!b=-f=M: fg=lmg bg=grn !)(!b=-f=C: fg=lcy bg=grn !)(!b=-f=-: fg=lst bg=grn !)");
	brrlog(0, "(!b=yf=0: fg=nrm bg=ylw !)(!b=-f=k: fg=blk bg=ylw !)(!b=-f=r: fg=red bg=ylw !)(!b=-f=g: fg=grn bg=ylw !)(!b=-f=y: fg=ylw bg=ylw !)(!b=-f=b: fg=blu bg=ylw !)(!b=-f=m: fg=mgn bg=ylw !)(!b=-f=c: fg=cyn bg=ylw !)(!b=-f=w: fg=wht bg=ylw !)(!b=-f=-K: fg=gry bg=ylw !)(!b=-f=R: fg=lrd bg=ylw !)(!b=-f=G: fg=lgr bg=ylw !)(!b=-f=Y: fg=lyw bg=ylw !)(!b=-f=B: fg=lbl bg=ylw !)(!b=-f=M: fg=lmg bg=ylw !)(!b=-f=C: fg=lcy bg=ylw !)(!b=-f=-: fg=lst bg=ylw !)");
	brrlog(0, "(!b=bf=0: fg=nrm bg=blu !)(!b=-f=k: fg=blk bg=blu !)(!b=-f=r: fg=red bg=blu !)(!b=-f=g: fg=grn bg=blu !)(!b=-f=y: fg=ylw bg=blu !)(!b=-f=b: fg=blu bg=blu !)(!b=-f=m: fg=mgn bg=blu !)(!b=-f=c: fg=cyn bg=blu !)(!b=-f=w: fg=wht bg=blu !)(!b=-f=-K: fg=gry bg=blu !)(!b=-f=R: fg=lrd bg=blu !)(!b=-f=G: fg=lgr bg=blu !)(!b=-f=Y: fg=lyw bg=blu !)(!b=-f=B: fg=lbl bg=blu !)(!b=-f=M: fg=lmg bg=blu !)(!b=-f=C: fg=lcy bg=blu !)(!b=-f=-: fg=lst bg=blu !)");
	brrlog(0, "(!b=mf=0: fg=nrm bg=mgn !)(!b=-f=k: fg=blk bg=mgn !)(!b=-f=r: fg=red bg=mgn !)(!b=-f=g: fg=grn bg=mgn !)(!b=-f=y: fg=ylw bg=mgn !)(!b=-f=b: fg=blu bg=mgn !)(!b=-f=m: fg=mgn bg=mgn !)(!b=-f=c: fg=cyn bg=mgn !)(!b=-f=w: fg=wht bg=mgn !)(!b=-f=-K: fg=gry bg=mgn !)(!b=-f=R: fg=lrd bg=mgn !)(!b=-f=G: fg=lgr bg=mgn !)(!b=-f=Y: fg=lyw bg=mgn !)(!b=-f=B: fg=lbl bg=mgn !)(!b=-f=M: fg=lmg bg=mgn !)(!b=-f=C: fg=lcy bg=mgn !)(!b=-f=-: fg=lst bg=mgn !)");
	brrlog(0, "(!b=cf=0: fg=nrm bg=cyn !)(!b=-f=k: fg=blk bg=cyn !)(!b=-f=r: fg=red bg=cyn !)(!b=-f=g: fg=grn bg=cyn !)(!b=-f=y: fg=ylw bg=cyn !)(!b=-f=b: fg=blu bg=cyn !)(!b=-f=m: fg=mgn bg=cyn !)(!b=-f=c: fg=cyn bg=cyn !)(!b=-f=w: fg=wht bg=cyn !)(!b=-f=-K: fg=gry bg=cyn !)(!b=-f=R: fg=lrd bg=cyn !)(!b=-f=G: fg=lgr bg=cyn !)(!b=-f=Y: fg=lyw bg=cyn !)(!b=-f=B: fg=lbl bg=cyn !)(!b=-f=M: fg=lmg bg=cyn !)(!b=-f=C: fg=lcy bg=cyn !)(!b=-f=-: fg=lst bg=cyn !)");
	brrlog(0, "(!b=wf=0: fg=nrm bg=wht !)(!b=-f=k: fg=blk bg=wht !)(!b=-f=r: fg=red bg=wht !)(!b=-f=g: fg=grn bg=wht !)(!b=-f=y: fg=ylw bg=wht !)(!b=-f=b: fg=blu bg=wht !)(!b=-f=m: fg=mgn bg=wht !)(!b=-f=c: fg=cyn bg=wht !)(!b=-f=w: fg=wht bg=wht !)(!b=-f=-K: fg=gry bg=wht !)(!b=-f=R: fg=lrd bg=wht !)(!b=-f=G: fg=lgr bg=wht !)(!b=-f=Y: fg=lyw bg=wht !)(!b=-f=B: fg=lbl bg=wht !)(!b=-f=M: fg=lmg bg=wht !)(!b=-f=C: fg=lcy bg=wht !)(!b=-f=-: fg=lst bg=wht !)");
	brrlog(0, "(!b=Kf=0: fg=nrm bg=gry !)(!b=-f=k: fg=blk bg=gry !)(!b=-f=r: fg=red bg=gry !)(!b=-f=g: fg=grn bg=gry !)(!b=-f=y: fg=ylw bg=gry !)(!b=-f=b: fg=blu bg=gry !)(!b=-f=m: fg=mgn bg=gry !)(!b=-f=c: fg=cyn bg=gry !)(!b=-f=w: fg=wht bg=gry !)(!b=-f=-K: fg=gry bg=gry !)(!b=-f=R: fg=lrd bg=gry !)(!b=-f=G: fg=lgr bg=gry !)(!b=-f=Y: fg=lyw bg=gry !)(!b=-f=B: fg=lbl bg=gry !)(!b=-f=M: fg=lmg bg=gry !)(!b=-f=C: fg=lcy bg=gry !)(!b=-f=-: fg=lst bg=gry !)");
	brrlog(0, "(!b=Rf=0: fg=nrm bg=lrd !)(!b=-f=k: fg=blk bg=lrd !)(!b=-f=r: fg=red bg=lrd !)(!b=-f=g: fg=grn bg=lrd !)(!b=-f=y: fg=ylw bg=lrd !)(!b=-f=b: fg=blu bg=lrd !)(!b=-f=m: fg=mgn bg=lrd !)(!b=-f=c: fg=cyn bg=lrd !)(!b=-f=w: fg=wht bg=lrd !)(!b=-f=-K: fg=gry bg=lrd !)(!b=-f=R: fg=lrd bg=lrd !)(!b=-f=G: fg=lgr bg=lrd !)(!b=-f=Y: fg=lyw bg=lrd !)(!b=-f=B: fg=lbl bg=lrd !)(!b=-f=M: fg=lmg bg=lrd !)(!b=-f=C: fg=lcy bg=lrd !)(!b=-f=-: fg=lst bg=lrd !)");
	brrlog(0, "(!b=Gf=0: fg=nrm bg=lgr !)(!b=-f=k: fg=blk bg=lgr !)(!b=-f=r: fg=red bg=lgr !)(!b=-f=g: fg=grn bg=lgr !)(!b=-f=y: fg=ylw bg=lgr !)(!b=-f=b: fg=blu bg=lgr !)(!b=-f=m: fg=mgn bg=lgr !)(!b=-f=c: fg=cyn bg=lgr !)(!b=-f=w: fg=wht bg=lgr !)(!b=-f=-K: fg=gry bg=lgr !)(!b=-f=R: fg=lrd bg=lgr !)(!b=-f=G: fg=lgr bg=lgr !)(!b=-f=Y: fg=lyw bg=lgr !)(!b=-f=B: fg=lbl bg=lgr !)(!b=-f=M: fg=lmg bg=lgr !)(!b=-f=C: fg=lcy bg=lgr !)(!b=-f=-: fg=lst bg=lgr !)");
	brrlog(0, "(!b=Yf=0: fg=nrm bg=lyw !)(!b=-f=k: fg=blk bg=lyw !)(!b=-f=r: fg=red bg=lyw !)(!b=-f=g: fg=grn bg=lyw !)(!b=-f=y: fg=ylw bg=lyw !)(!b=-f=b: fg=blu bg=lyw !)(!b=-f=m: fg=mgn bg=lyw !)(!b=-f=c: fg=cyn bg=lyw !)(!b=-f=w: fg=wht bg=lyw !)(!b=-f=-K: fg=gry bg=lyw !)(!b=-f=R: fg=lrd bg=lyw !)(!b=-f=G: fg=lgr bg=lyw !)(!b=-f=Y: fg=lyw bg=lyw !)(!b=-f=B: fg=lbl bg=lyw !)(!b=-f=M: fg=lmg bg=lyw !)(!b=-f=C: fg=lcy bg=lyw !)(!b=-f=-: fg=lst bg=lyw !)");
	brrlog(0, "(!b=Bf=0: fg=nrm bg=lbl !)(!b=-f=k: fg=blk bg=lbl !)(!b=-f=r: fg=red bg=lbl !)(!b=-f=g: fg=grn bg=lbl !)(!b=-f=y: fg=ylw bg=lbl !)(!b=-f=b: fg=blu bg=lbl !)(!b=-f=m: fg=mgn bg=lbl !)(!b=-f=c: fg=cyn bg=lbl !)(!b=-f=w: fg=wht bg=lbl !)(!b=-f=-K: fg=gry bg=lbl !)(!b=-f=R: fg=lrd bg=lbl !)(!b=-f=G: fg=lgr bg=lbl !)(!b=-f=Y: fg=lyw bg=lbl !)(!b=-f=B: fg=lbl bg=lbl !)(!b=-f=M: fg=lmg bg=lbl !)(!b=-f=C: fg=lcy bg=lbl !)(!b=-f=-: fg=lst bg=lbl !)");
	brrlog(0, "(!b=Mf=0: fg=nrm bg=lmg !)(!b=-f=k: fg=blk bg=lmg !)(!b=-f=r: fg=red bg=lmg !)(!b=-f=g: fg=grn bg=lmg !)(!b=-f=y: fg=ylw bg=lmg !)(!b=-f=b: fg=blu bg=lmg !)(!b=-f=m: fg=mgn bg=lmg !)(!b=-f=c: fg=cyn bg=lmg !)(!b=-f=w: fg=wht bg=lmg !)(!b=-f=-K: fg=gry bg=lmg !)(!b=-f=R: fg=lrd bg=lmg !)(!b=-f=G: fg=lgr bg=lmg !)(!b=-f=Y: fg=lyw bg=lmg !)(!b=-f=B: fg=lbl bg=lmg !)(!b=-f=M: fg=lmg bg=lmg !)(!b=-f=C: fg=lcy bg=lmg !)(!b=-f=-: fg=lst bg=lmg !)");
	brrlog(0, "(!b=Cf=0: fg=nrm bg=lcy !)(!b=-f=k: fg=blk bg=lcy !)(!b=-f=r: fg=red bg=lcy !)(!b=-f=g: fg=grn bg=lcy !)(!b=-f=y: fg=ylw bg=lcy !)(!b=-f=b: fg=blu bg=lcy !)(!b=-f=m: fg=mgn bg=lcy !)(!b=-f=c: fg=cyn bg=lcy !)(!b=-f=w: fg=wht bg=lcy !)(!b=-f=-K: fg=gry bg=lcy !)(!b=-f=R: fg=lrd bg=lcy !)(!b=-f=G: fg=lgr bg=lcy !)(!b=-f=Y: fg=lyw bg=lcy !)(!b=-f=B: fg=lbl bg=lcy !)(!b=-f=M: fg=lmg bg=lcy !)(!b=-f=C: fg=lcy bg=lcy !)(!b=-f=-: fg=lst bg=lcy !)");
	brrlog(0, "(!b=Wf=0: fg=nrm bg=lwh !)(!b=-f=k: fg=blk bg=lwh !)(!b=-f=r: fg=red bg=lwh !)(!b=-f=g: fg=grn bg=lwh !)(!b=-f=y: fg=ylw bg=lwh !)(!b=-f=b: fg=blu bg=lwh !)(!b=-f=m: fg=mgn bg=lwh !)(!b=-f=c: fg=cyn bg=lwh !)(!b=-f=w: fg=wht bg=lwh !)(!b=-f=-K: fg=gry bg=lwh !)(!b=-f=R: fg=lrd bg=lwh !)(!b=-f=G: fg=lgr bg=lwh !)(!b=-f=Y: fg=lyw bg=lwh !)(!b=-f=B: fg=lbl bg=lwh !)(!b=-f=M: fg=lmg bg=lwh !)(!b=-f=C: fg=lcy bg=lwh !)(!b=-f=-: fg=lst bg=lwh !)");
	brrlog(0, "(!b=-f=0: fg=nrm bg=lst !)(!b=-f=k: fg=blk bg=lst !)(!b=-f=r: fg=red bg=lst !)(!b=-f=g: fg=grn bg=lst !)(!b=-f=y: fg=ylw bg=lst !)(!b=-f=b: fg=blu bg=lst !)(!b=-f=m: fg=mgn bg=lst !)(!b=-f=c: fg=cyn bg=lst !)(!b=-f=w: fg=wht bg=lst !)(!b=-f=-K: fg=gry bg=lst !)(!b=-f=R: fg=lrd bg=lst !)(!b=-f=G: fg=lgr bg=lst !)(!b=-f=Y: fg=lyw bg=lst !)(!b=-f=B: fg=lbl bg=lst !)(!b=-f=M: fg=lmg bg=lst !)(!b=-f=C: fg=lcy bg=lst !)(!b=-f=-: fg=lst bg=lst !)");
#endif
	brrlog_deinit();
}
#endif
