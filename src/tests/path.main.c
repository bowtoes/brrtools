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

int main(int argc, char **argv)
{
	brrstgT path = {0};
	int error = 0;
	brrlog_init();
	if (argc < 2) {
		BRRLOG_NOR("Please insert a path.");
		return 0;
	}

	if (brrstg_new(&path, argv[1], -1)) {
		BRRLOG_ERR("Failed to initialize search path : %s", strerror(errno));
		return 1;
	} else {
		brrpath_walk_optionsT opt = {0};
		brrpath_walk_resultT result = {0};
		opt.path = path;
		opt.max_depth = 4;
		opt.min_depth = 2;
		error = brrpath_walk(&result, &opt, NULL);
		if (!error && !errno) {
			BRRLOG_NOR("Found %zu paths", result.result_count);
			for (brrsz i = 0; i < result.result_count; ++i) {
				brrpath_infoT *info = &result.walk_results[i];
				BRRLOG_NORN("%5zu %d %*.*s", i, info->depth, 2*info->depth, 2*info->depth, "");
				BRRLOG_MESSAGESN(gbrrlog_type_normal.level,
				    info->type == brrpath_type_directory?brrlog_color_cyan:
					info->type == brrpath_type_file?brrlog_color_yellow:
					info->type == brrpath_type_other?brrlog_color_magenta:
					brrlog_color_darkgrey, brrlog_color_normal, brrlog_style_bold, brrlog_font_normal,
				    "%s", pathtypes[info->type]);
				BRRLOG_NORNP(" %-*s : ", 60-info->depth, BRRTIL_NULSTR((char *)info->full_path.opaque));
				BRRLOG_MESSAGESN(gbrrlog_type_normal.level,
				    brrlog_color_blue, brrlog_color_normal, brrlog_style_normal, brrlog_font_normal,
				    "DIR");
				BRRLOG_NORNP(" %-40s ", BRRTIL_NULSTR((char *)info->directory.opaque));
				BRRLOG_MESSAGESN(gbrrlog_type_normal.level,
				    brrlog_color_red, brrlog_color_normal, brrlog_style_normal, brrlog_font_normal,
				    "BSE");
				BRRLOG_NORNP(" %-40s ", BRRTIL_NULSTR((char *)info->base_name.opaque));
				BRRLOG_MESSAGESN(gbrrlog_type_normal.level,
				    brrlog_color_green, brrlog_color_normal, brrlog_style_normal, brrlog_font_normal,
				    "EXT");
				BRRLOG_NORNP(" %-8s", BRRTIL_NULSTR((char *)info->extension.opaque));
				if (info->type != brrpath_type_directory) {
					BRRLOG_NORNP(" %zu", info->size);
				}
				BRRLOG_NORP("");
			}
		} else {
			BRRLOG_ERROR("ERROR %d (%d, %s)", error, errno, strerror(errno));
		}
		brrpath_walk_result_delete(&result);
	}
	brrstg_delete(&path);
	brrlog_deinit();
	return error;
}
