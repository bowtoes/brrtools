/*
 * This test file is unlike the others
 *
 * brrpath is too complicated for me to implement automated tests, so it needs
 * manual verification.
 *
 * This is a 'main.c' file.
 * */
#include <stdio.h>

#include <errno.h>
#include <string.h>

#include "brrtools/brrlib.h"
#include "brrtools/brrlog.h"
#include "brrtools/brrpath.h"
#include "brrtools/brrtil.h"

static const char * pathtypes[] = {
	"NON",
	"REG",
	"DIR",
	"LNK",
	"OTH"
};

static void BRRCALL
log_info(const brrpath_infoT *const info, brrsz i)
{
	BRRLOG_NORN("%5zu %zu %*.*s", i, info->depth, 2*info->depth, 2*info->depth, "");
	BRRLOG_MESSAGEFN(gbrrlog_type_normal.level,
	    info->type == brrpath_type_directory?brrlog_color_cyan:
		info->type == brrpath_type_file?brrlog_color_yellow:
		info->type == brrpath_type_other?brrlog_color_magenta:
		brrlog_color_red, brrlog_color_normal, brrlog_style_bold, brrlog_font_normal,
	    "%s", pathtypes[info->type]);
	BRRLOG_NORNP(" %-*s : ", 76-2*info->depth, BRRTIL_NULSTR((char *)info->full_path.opaque));
	BRRLOG_MESSAGEFN(gbrrlog_type_normal.level,
	    brrlog_color_blue, brrlog_color_normal, brrlog_style_normal, brrlog_font_normal,
	    "DIR");
	BRRLOG_NORNP(" %-60s ", BRRTIL_NULSTR((char *)info->components.directory.opaque));
	BRRLOG_MESSAGEFN(gbrrlog_type_normal.level,
	    brrlog_color_red, brrlog_color_normal, brrlog_style_normal, brrlog_font_normal,
	    "BSE");
	BRRLOG_NORNP(" %-20s ", BRRTIL_NULSTR((char *)info->components.base_name.opaque));
	BRRLOG_MESSAGEFN(gbrrlog_type_normal.level,
	    brrlog_color_green, brrlog_color_normal, brrlog_style_normal, brrlog_font_normal,
	    "EXT");
	BRRLOG_NORNP(" %-8s", BRRTIL_NULSTR((char *)info->components.extension.opaque));
	if (info->type != brrpath_type_directory) {
		BRRLOG_NORNP(" %zu", info->size);
	}
	BRRLOG_NORP("");
}

#if 0 /* Test path_info_new */
int main(int argc, char **argv)
{
	if (brrlog_init()) {
		printf("Failed to initialize log : %s", strerror(errno));
		return 1;
	} else if (argc <= 1) {
		BRRLOG_NOR("Please insert at least one path");
		return 0;
	}
	for (int i = 1; i < argc; ++i) {
		char *arg = argv[i];
		brrstgT test = {0};
		brrpath_infoT res = {0};
		if (brrstg_new(&test, arg, -1)) {
			BRRDEBUG_TRACE("Failed to initialize arg string '%s' : %s", arg, strerror(errno));
		} else if (brrpath_info_new(&res, &test)) {
			BRRDEBUG_TRACE("Failed to initialize arg info '%s' : %s", arg, strerror(errno));
		}

		log_info(&res, i);

		brrpath_info_delete(&res);
		brrstg_delete(&test);
	}
}
#else /* Test path_walk */
int main(int argc, char **argv)
{
	int error = 0;
	brrpath_walk_optionsT opt = {0};
	brrpath_walk_resultT result = {0};
	const char *path = NULL;
	brrlog_init();
	if (argc < 2) {
		BRRLOG_NOR("Please insert a path.");
		return 0;
	}
	path = argv[1];

	opt.min_depth = 1;
	opt.max_depth = 2;
	error = brrpath_walk(path, &result, &opt, NULL);
	if (!error && !errno) {
		BRRLOG_NOR("Found %zu paths", result.result_count);
		for (brrsz i = 0; i < result.result_count; ++i) {
			brrpath_infoT *info = &result.walk_results[i];
			BRRLOG_NORN("%5zu %d %*.*s", i, info->depth, 2*info->depth, 2*info->depth, "");
			BRRLOG_STYLENP(info->type == brrpath_type_directory?brrlog_color_cyan:
				info->type == brrpath_type_file?brrlog_color_yellow:
				info->type == brrpath_type_other?brrlog_color_magenta:
				brrlog_color_darkgrey, -1, brrlog_style_bold, "%s", pathtypes[info->type]);
			BRRLOG_NORNP(" %-*s : ", 76-2*info->depth, BRRTIL_NULSTR((char *)info->full_path.opaque));
			BRRLOG_FORENP(brrlog_color_blue, "DIR");
			BRRLOG_NORNP(" %-60s ", BRRTIL_NULSTR((char *)info->components.directory.opaque));
			BRRLOG_FORENP(brrlog_color_red, "BSE");
			BRRLOG_NORNP(" %-20s ", BRRTIL_NULSTR((char *)info->components.base_name.opaque));
			BRRLOG_FORENP(brrlog_color_green, "EXT");
			BRRLOG_NORNP(" %-8s", BRRTIL_NULSTR((char *)info->components.extension.opaque));
			if (info->type != brrpath_type_directory) {
				BRRLOG_NORNP(" %zu", info->size);
			}
			BRRLOG_NORP("");
		}
	} else {
		BRRLOG_ERROR("ERROR %d (%d, %s)", error, errno, strerror(errno));
	}
	brrpath_walk_result_delete(&result);

	brrlog_deinit();
	return error;
}
#endif
