#include <stdio.h>
#include <stdlib.h>

#include <brrtools/brrlog.h>
#include <brrtools/brrlib.h>
#include <brrtools/brrmem.h>
#include <brrtools/brrpath.h>
#include <brrtools/brrbuffer.h>

#include <errno.h>
#include <string.h>

static void logtext(void) {
	BRRLOG_CRI("%zu>%zu: %zu CRI", brrlog_minpriority(), brrlog_maxpriority(), brrlog_format_critical.level.priority);
	BRRLOG_ERR("%zu>%zu: %zu ERR", brrlog_minpriority(), brrlog_maxpriority(), brrlog_format_error.level.priority);
	BRRLOG_NOR("%zu>%zu: %zu NOR", brrlog_minpriority(), brrlog_maxpriority(), brrlog_format_normal.level.priority);
	BRRLOG_WAR("%zu>%zu: %zu WAR", brrlog_minpriority(), brrlog_maxpriority(), brrlog_format_warning.level.priority);
	BRRLOG_DEB("%zu>%zu: %zu DEB", brrlog_minpriority(), brrlog_maxpriority(), brrlog_format_debug.level.priority);
	BRRLOG_MESSAGE(brrlog_priority_normal, brrlog_destination_stdout, "Custom PREFIX:", brrlog_color_last, brrlog_color_cyan, brrlog_style_bold, brrlog_font_normal, "Woaho!");
	BRRLOG_MESSAGE(brrlog_priority_normal, brrlog_destination_stdout, "Custom PREFIX:", brrlog_color_red, brrlog_color_last, brrlog_style_under, brrlog_font_normal, "Woaho!");
	BRRLOG_MESSAGE(brrlog_priority_normal, brrlog_destination_stdout, "Custom PREFIX:", brrlog_color_green, brrlog_color_white, brrlog_style_last, brrlog_font_2, "Woaho!");
	BRRLOG_MESSAGE(brrlog_priority_normal, brrlog_destination_stdout, "Custom PREFIX:", brrlog_color_yellow, brrlog_color_lightwhite, brrlog_style_fraktur, brrlog_font_last, "Woaho!");
	BRRLOG_MESSAGE(brrlog_priority_normal, brrlog_destination_stdout, "Custom PREFIX:", brrlog_color_lightblue, brrlog_color_red, brrlog_style_frame, brrlog_font_4, "Woaho!");
	BRRLOG_MESSAGE(brrlog_priority_normal, brrlog_destination_stdout, NULL, brrlog_color_black, brrlog_color_lightcyan, brrlog_style_circle, brrlog_font_4, "No PREFIX");
}
static void testlogtext(void) {
	brru8 min = brrlog_minpriority(), max = brrlog_maxpriority();
	BRRLOG_NORP("TEXT TEST");
	logtext();
	brrlog_setminpriority(brrlog_priority_warning);
	logtext();
	brrlog_setmaxpriority(brrlog_priority_error);
	logtext();
	brrlog_setminpriority(min);
	brrlog_setmaxpriority(max);
}
static void logdigits(char sep, brrsz spc, brrb1 sgn, brrb1 neg) {
	for (brru8 i = 0, num = 0; i < 10;num = num * 10 + ++i) {
		BRRLOG_NORNP("OTHR : ");
		BRRLOGD_NORNP(sep, spc, sgn, (1-(2*neg))*num, 10);
		BRRLOG_NORNP("\tCORR : ");
		BRRLOG_NORP(sgn?"%-10lld":"%-10llu", (1-(2*neg))*num);
	}
}
static void testlogdigits(void) {
	BRRLOG_NORP("DIGIT TEST");
	BRRLOG_NORP("SEP ',' SPC 3 NOSGN POS : ");
	logdigits(',', 3, false, false);
	BRRLOG_NORP("SEP  0  SPC 0 NOSGN POS : ");
	logdigits(0,   0, false, false);
	BRRLOG_NORP("SEP '.' SPC 4 NOSGN POS : ");
	logdigits('.', 4, false, false);
	BRRLOG_NORP("SEP '.' SPC 4   SGN POS : ");
	logdigits('.', 4, true, false);
	BRRLOG_NORP("SEP '.' SPC 4 NOSGN NEG : ");
	logdigits('.', 4, false, true);
	BRRLOG_NORP("SEP '.' SPC 4   SGN NEG : ");
	logdigits('.', 4, true, true);
}
#define GETBIT(n, b) ((n&(UINT64_C(1)<<(b)))>>(b))
static void testlogbits(void) {
	BRRLOG_NORP("BITS TEST");
	brru8 num = brrlib_rand();
	BRRLOG_NORNP("NUM: ");
	BRRLOGD_NORP(',', 3, false, num, 10);
	BRRLOG_NORNP("BITS : ");
	for (brru8 i = 0; i < 64; ++i) {
		if (i&&(i%8==0)) BRRLOG_NORNP(".");
		BRRLOG_NORNP("%i", (num & (UINT64_C(1)<<i))>>i);
	}
	BRRLOG_NORP("");
	for (brrs1 i = 8; i >= 0; --i) {
		for (brrs1 j = 8; j >= 0; --j) {
			char sp = '.'-(8-j);
			BRRLOG_NORNP("B %2zu S '%c' SPC %zu NB Nb : ", j * 8, sp, i);
			BRRLOGB_NORP(0, 0, j * 8, &num, sp, i);
			BRRLOG_NORNP("B %2zu S '%c' SPC %zu NB Rb : ", j * 8, sp, i);
			BRRLOGB_NORP(0, 1, j * 8, &num, sp, i);
			BRRLOG_NORNP("B %2zu S '%c' SPC %zu RB Nb : ", j * 8, sp, i);
			BRRLOGB_NORP(1, 0, j * 8, &num, sp, i);
			BRRLOG_NORNP("B %2zu S '%c' SPC %zu RB Rb : ", j * 8, sp, i);
			BRRLOGB_NORP(1, 1, j, &num, sp, i);
		}
	}
}
static void testtime(void) {
	BRRLOG_NORP("TIME TEST");
	for (brru8 i = 0; i < 20; ++i) {
		BRRLOG_NORP("Time %2i: %f", i+1, (double)brrlib_utime() / 1000000);
		brrlib_usleep(100000);
	}
}
static void testrand(void) {
	BRRLOG_NORP("RAND TEST");
	for (brru8 i = 0; i < 200; ++i) {
		BRRLOG_NORNP("RND        : ");
		BRRLOG_NORP("0x%016llx", brrlib_rand());
	}
	for (brru8 i = 0; i < 20; ++i) {
		brrlib_srand(i);
		for (brru8 j = 0; j < 10; ++j) {
			BRRLOG_NORNP("SRND %2zu+%2zu : ", i+1, j+1);
			BRRLOG_ERRP("0x%016llx", brrlib_rand());
		}
	}
	for (brru8 i = 0; i < 200; ++i) {
		BRRLOG_NORNP("TRND %3zu : ", i+1);
		BRRLOG_ERRP("0x%016llx", brrlib_trand());
	}
}

static void testbases(void) {
	struct temp {
		brrb1 issigned;
		brru8 num;
	};
	static const struct temp nums[] = {
		{0,          0}, {1,          0},
		{0,        877}, {1,        877},
		{0,       -877}, {1,       -877},
		{0,      19391}, {1,      19391},
		{0,     -19391}, {1,     -19391},
		{0,     -19391}, {1,     -19391},
		{0, -(INT64_MIN+1)}, {1, -(INT64_MIN+1)},
		{0,  INT64_MIN}, {1,  INT64_MIN},
	};
	static char str[8 * sizeof(brru8) + 1 + 1] = {0};
	for (brru2 base = 0; base <= brrlib_max_base(); ++base) {
		BRRLOG_NORP("<-------- BASE   %zu", base);
		BRRLOG_NORNP("<-------- DIGITS ");
		for (brru1 b = 0; b < base; ++b)
			BRRLOG_NORNP("%c", brrlib_bases[b]);
		BRRLOG_NORP("");
		for (brru8 i = 0; i < (sizeof(nums) / sizeof(struct temp)); ++i) {
			brrsz print = brrlib_print_base(str, sizeof(str), nums[i].num, nums[i].issigned, base);
			BRRLOG_NORNP("%s NUM ", nums[i].issigned?"SIGN":"    ");
			BRRLOG_NORNP(nums[i].issigned?"%21lli":"%21llu", nums[i].issigned?(brrs8)nums[i].num:nums[i].num);
			BRRLOG_NORP(", PRINTED %2zu :%65s", print, str);
		}
	}
}

static void testuntilfrom(void) {
	static const char data[] = "Some test data, I think.";
	static const brrsz ds = sizeof(data);
	static brrsz ft,st,tt,Ft,pt,fc,nt,nc,lt;
	ft = brrmem_next(data, ds-1, 't', 0);
	st = brrmem_next(data, ds-1, 't', ft+1);
	tt = brrmem_next(data, ds-1, 't', st+1);
	Ft = brrmem_next(data, ds-1, 't', tt+1);
	pt = brrmem_previous(data, ds-1, 't', Ft-1);
	fc = brrmem_previous(data, ds-1, ',', Ft+1);
	nt = brrmem_next(data, ds-1, 't', Ft+1);
	nc = brrmem_next(data, ds-1, ',', fc+1);
	lt = brrmem_previous(data, ds-1, 't', ds-1);
	BRRLOG_NORP("Data: '%s' (%2zu)", data, ds);
	BRRLOG_NORP("First  't' : %2zu '%s'", ft, data+ft);
	BRRLOG_NORP("Second 't' : %2zu '%s'", st, data+st);
	BRRLOG_NORP("Third  't' : %2zu '%s'", tt, data+tt);
	BRRLOG_NORP("Fourth 't' : %2zu '%s'", Ft, data+Ft);
	BRRLOG_NORP("Prev   't' : %2zu '%s'", pt, data+pt);
	BRRLOG_NORP("Last   't' : %2zu '%s'", lt, data+lt);
	BRRLOG_NORP("Before ',' : %2zu '%s'", fc, data+fc);
	BRRLOG_NORP("No     't' : %2zu '%s'", nt, data+nt);
	BRRLOG_NORP("No     ',' : %2zu '%s'", nc, data+nc);
}

static void testbuffer(void) {
	static const char ma[] = "persnickity";
	static const char mb[] = "|nice";
	brrbufferT buff = brrbuffer_new(100);
	if (buff.opaque) {
		brrsz wt = 0;
		BRRLOG_NOR("NEW BUFFER: size %zu cap %zu", buff.size, brrbuffer_capacity(&buff));
		wt = brrbuffer_write(&buff, ma, sizeof(ma));
		BRRLOG_NOR("Wrote %zu to buffer, new size %zu cap %zu : '%s'", wt, buff.size, brrbuffer_capacity(&buff), brrbuffer_data(&buff));
		for (brrsz i = 0; i < 20; ++i) {
			buff.position--;
			wt = brrbuffer_write(&buff, mb, sizeof(mb));
			BRRLOG_NOR("Wrote %zu to buffer, new size %zu cap %zu : '%s'", wt, buff.size, brrbuffer_capacity(&buff), brrbuffer_data(&buff));
		}
	} else {
		BRRLOG_ERR("Error creating buffer: %s", strerror(errno));
	}
	brrbuffer_delete(&buff);
}
static void testbufffind(void) {
	static const char ma[] = "There are a lot of a's in this sentence.a";
	static const char key[] = "a";
	brrbufferT buffer = brrbuffer_new(sizeof(ma));
	if (buffer.opaque) {
		brrbuffer_write(&buffer, ma, sizeof(ma));
		buffer.position = 0;
		BRRLOG_NOR("Wrote to buffer : '%s'", brrbuffer_stream(&buffer));
		while (brrbuffer_find_next(&buffer, key, sizeof(key) - 1)) {
			BRRLOG_NOR("Found forward  key : '%s'", brrbuffer_stream(&buffer));
			buffer.position++;
		}
		while (brrbuffer_find_previous(&buffer, key, sizeof(key) - 1)) {
			BRRLOG_NOR("Found backward key : '%s'", brrbuffer_stream(&buffer));
			buffer.position--;
		}
	} else {
		BRRLOG_ERR("Error creating buffer: %s", strerror(errno));
	}
}

int main(void)
{
	brrlog_setlogmax(0);
	brrlogctl_styleon = false;
	brrlogctl_debugon = true;
	brrlogctl_flushon = true;
	testbufffind();
#if 0
	brrlib_use_extended_bases = true;
	testbases();
#endif
#if 0
	brrlib_clear();
	testuntilfrom();
	brrlib_pause();
	brrlib_clear();
	testmemapppre();
#endif
#if 0 // Lib/Log tests
	testlogtext();
	brrlib_pause();
	brrlib_clear();
	testlogdigits();
	brrlib_pause();
	brrlib_clear();
	testlogbits();
	brrlib_pause();
	brrlib_clear();
	testtime();
	brrlib_pause();
	brrlib_clear();
	testrand();
	brrlib_pause();
	brrlib_clear();
#endif
}
