#include <stdio.h>
#include <stdlib.h>

#include <brrtools/brrlog.h>
#include <brrtools/brrlib.h>

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
		BRRLOGD_NORNP(sep, spc, sgn, (1-(2*neg))*num);
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
	BRRLOGD_NORP(',', 3, false, num);
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

int main(void)
{
	brrlog_setlogmax(0);
	brrlog_styleon = true;
	brrlog_debugon = true;
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
