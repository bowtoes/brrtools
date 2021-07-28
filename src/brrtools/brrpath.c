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

#include "brrtools/brrpath.h"

#include <errno.h>
#include <stdio.h>
#if defined(BRRPLATFORMTYPE_WINDOWS)
#include <windows.h>
#elif defined(BRRPLATFORMTYPE_POSIX)
#include <ftw.h>
#elif defined(BRRPLATFORMTYPE_UNIX)
#error UNIX system without POSIX, dont know if <ftw.h>
#else
#error Unknown system
#endif

#if defined(BRRPLATFORMTYPE_UNIX)
#include <sys/stat.h> /* TODO *nix universal? */
#endif

#include "brrtools/brrlib.h"
#include "brrtools/brrmem.h"
#include "brrtools/brrtil.h"
#include "brrtools/noinstall/statics.h"

void BRRCALL
brrpath_walk_result_delete(brrpath_walk_resultT *const result)
{
	if (result) {
		if (result->walk_results) {
			for (brrsz i = 0; i < result->result_count; ++i) {
				brrpath_info_delete(&(result->walk_results[i]));
			}
			brrlib_alloc((void **)&result->walk_results, 0, 0);
		}
	}
}

static brrpath_typeT BRRCALL
int_determine_type(brru8 attr)
{
	brrpath_typeT type = brrpath_type_none;
#if defined(BRRPLATFORMTYPE_WINDOWS)
	type = (attr & FILE_ATTRIBUTE_DIRECTORY)?brrpath_type_directory:
		(attr & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_VIRTUAL))?brrpath_type_other:
		brrpath_type_file;
#else
	type = S_ISDIR(attr)?brrpath_type_directory:
		!S_ISREG(attr)?brrpath_type_other:
		brrpath_type_file;
#endif
	return type;
}
int BRRCALL
brrpath_stat(const brrstgT *const path, brrpath_stat_resultT *const st)
{
	int error = 0;
	if (!path || !st) {
		return 0;
	} else if (!path->opaque) {
		return 1;
	} else {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		WIN32_FILE_ATTRIBUTE_DATA attr = {0};
		if (!GetFileAttributesEx(path->opaque, GetFileExInfoStandard, &attr)) {
			error = GetLastError();
			if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
				/* Path doesn't exist. */
				error = 0;
			} else {
				error = -1;
			}
			*st = (brrpath_stat_resultT){0};
		} else {
			st->exists = 1;
			st->type = int_determine_type(attr.dwFileAttributes);
			st->size = BRRTIL_WIN_ULRG(attr.nFileSizeLow, attr.nFileSizeHigh).QuadPart;
		}
#else
		struct stat s = {0};
		if (stat(path->opaque, &s)) {
			error = errno;
			if (error == ENOENT) {
				/* Path doesn't exist. */
				error = 0;
			} else {
				error = -1;
			}
			*st = (brrpath_stat_resultT){0};
		} else {
			st->exists = 1;
			st->size = s.st_size;
			st->type = int_determine_type(s.st_mode);
		}
#endif
	}
	return error;
}

static int BRRCALL
int_res_fill(brrpath_infoT *const res, const char *const fpath, const char *const base_name, const brrpath_stat_resultT *const st)
{
	int err = 0;
	if (!(err = brrstg_new(&res->base_name, base_name, BRRPATH_MAX_NAME))) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		if (!(err = brrstg_new(&res->directory, fpath, BRRPATH_MAX_PATH)))
#else
		if (!(err = brrstg_new(&res->full_path, fpath, BRRPATH_MAX_PATH)))
#endif
		{
#if defined(BRRPLATFORMTYPE_WINDOWS)
			if (!(err = brrstg_new(&res->full_path, NULL, 0)) &&
			    !(err = brrstg_print(&res->full_path, BRRPATH_MAX_PATH, 0, NULL, "%s\\%s", fpath, base_name)))
#else
			if (!(err = brrstg_new(&res->directory, res->full_path.opaque, res->full_path.length - res->base_name.length)))
#endif
			{
				if (st->type == brrpath_type_directory) {
					err = brrstg_new(&res->extension, NULL, 0);
				} else {
					brrsz end = brrmem_search_reverse(res->base_name.opaque,
					    res->base_name.length, ".", 1, res->base_name.length - 1);
					if (end && end != res->base_name.length)
						err = brrstg_new(&res->extension, (char*)res->base_name.opaque + end + 1, BRRPATH_MAX_NAME - end);
					else
						err = brrstg_new(&res->extension, NULL, 0);
				}
			}
		}
	}
	if (err) {
		brrpath_info_delete(res);
	} else {
		res->exists = st->exists;
		res->type = !st->exists?brrpath_type_none:st->type;
		res->size = st->size;
	}
	return err;
}
static int BRRCALL
int_char_is_sep(char ch)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	return ch=='/'||ch=='\\';
#else
	return ch=='/';
#endif
}
static int BRRCALL
int_char_is_valid_path(char ch)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	return !(ch=='<'||ch=='>'||
			ch==':'||ch=='"'||
			ch=='|'||ch=='?'||
			ch=='*'||ch<32);
#else
	return 1;
#endif
}
/* TODO where else should paths be cleaned? What can be passed into 'brrpath_walk'?
 * What will be pruned by erroring out at the stat? I hate paths. */
/* remove duplicate separators/invalid characters */
static int BRRCALL
int_path_clean(brrstgT *const path)
{
	char *p = (char *)path->opaque;
	brrsz nl = 0;
	for (brrsz i = 0; i < path->length; ++i) {
		if (!int_char_is_sep(p[i]) || (i + 1 < path->length && !int_char_is_sep(p[i+1])) || nl == 0) {
			if (int_char_is_valid_path(p[i])) {
				p[nl++] = p[i];
			}
		}
	}
	return brrsizestr(path, nl)?0:-1;
}
int BRRCALL
brrpath_info_new(brrpath_infoT *const info, const brrstgT *const path)
{
	if (!info || !path) {
		return 0;
	} else if (!path->opaque) {
		return 1;
	} else {
		brrpath_stat_resultT st = {0};
		int err = 0;
		brrpath_info_delete(info);
		if (!(err = brrpath_stat(path, &st))) {
			info->type = st.type;
			info->exists = st.exists;
			info->size = st.size;
			if (!(err = brrstg_new(&info->full_path, path->opaque, BRRPATH_MAX_PATH)) &&
				!(err = int_path_clean(&info->full_path))) {
				brrsz sep = brrmem_search_reverse(info->full_path.opaque, info->full_path.length,
				    BRRPATH_SEP_STR, 1, info->full_path.length - 1);
				if (sep == info->full_path.length) {
					err = brrstg_new(&info->directory, ".", 1);
					sep = 0;
				} else {
					err = brrstg_new(&info->directory, (char *)info->full_path.opaque, sep?sep:1);
					sep++;
				}
				if (!err && !(err = brrstg_new(&info->base_name, (char *)info->full_path.opaque + sep, BRRPATH_MAX_NAME))) {
					brrsz dot = brrmem_search_reverse(info->base_name.opaque, info->base_name.length,
					    ".", 1, info->base_name.length - 1);
					if (dot < info->base_name.length - 1) {
						err = brrstg_new(&info->extension, (char *)info->base_name.opaque + dot + 1, info->base_name.length - dot);
					} else {
						err = brrstg_new(&info->extension, NULL, 0);
					}
				}
			}
		}
		if (err)
			brrpath_info_delete(info);
		return 0;
	}
}
void BRRCALL
brrpath_info_delete(brrpath_infoT *const info)
{
	if (info) {
		brrstg_delete(&info->full_path);
		brrstg_delete(&info->directory);
		brrstg_delete(&info->base_name);
		brrstg_delete(&info->extension);
		info->depth = info->exists = info->size = info->type = 0;
	}
}
int BRRCALL
brrpath_info_copy(brrpath_infoT *const destination, const brrpath_infoT *const source)
{
	int err = 0;
	if (!destination || !source) {
		return 0;
	} else if (!(source->full_path.opaque && source->directory.opaque &&
	    source->base_name.opaque && source->extension.opaque)) {
		return 1;
	} else if (!(err = brrstg_copy(&destination->full_path, &source->full_path))) {
		if (!(err = brrstg_copy(&destination->directory, &source->directory))) {
			if (!(err = brrstg_copy(&destination->base_name, &source->base_name))) {
				if (!(err = brrstg_copy(&destination->extension, &source->extension))) {
					destination->size = source->size;
					destination->type = source->type;
					destination->exists = source->exists;
					destination->depth = 0;
				}
			}
		}
	}
	if (err) {
		brrpath_info_delete(destination);
	}
	return err;
}
int BRRCALL
brrpath_info_combine(brrstgT *const string, const brrpath_infoT *const info)
{
	if (!string || !info) {
		return 0;
	} else {
		if (info->extension.length) {
			return brrstg_print(string, BRRPATH_MAX_PATH, 0, NULL, "%s"BRRPATH_SEP_STR"%s.%s",
			    (char *)info->directory.opaque, (char *)info->base_name.opaque, (char *)info->extension.opaque);
		} else {
			return brrstg_print(string, BRRPATH_MAX_PATH, 0, NULL, "%s"BRRPATH_SEP_STR"%s",
			    (char *)info->directory.opaque, (char *)info->base_name.opaque);
		}
	}
}
int BRRCALL
brrpath_info_recombine(brrpath_infoT *const info)
{
	brrstgT temp = {0};
	if (!info) {
		return 0;
	} else if (brrstg_new(&temp, NULL, 0)) {
		return -1;
	} else {
		int err = 0;
		if (!(err = brrpath_info_combine(&temp, info))) {
			if ((err = brrstg_copy(&info->full_path, &temp)))
				brrstg_delete(&info->full_path);
		}
		brrstg_delete(&temp);
		return err;
	}
}

static int BRRCALL
int_add_res(brrpath_walk_resultT *const dst, const brrpath_infoT *const res)
{
	if (brrlib_alloc((void **)&dst->walk_results,
	    (dst->result_count + 1) * sizeof(brrpath_infoT), 0)) {
		return -1;
	}
	dst->walk_results[dst->result_count] = *res;
	dst->result_count++;
	return 0;
}

static brrpath_walk_resultT *walk_result;
static const brrpath_walk_optionsT *walk_options;
static int (*BRRCALL walk_filter)(const brrpath_infoT *const path_info);
#if defined(BRRPLATFORMTYPE_WINDOWS)
static int BRRCALL
int_walk_filt(const char *const fpath, brrsz current_depth)
{
	HANDLE start;
	WIN32_FIND_DATA find_data;
	char path[BRRPATH_MAX_PATH+1] = {0};
	snprintf(path, BRRPATH_MAX_PATH+1, "%s\\*.*", fpath);
	if ((start = FindFirstFile(path, &find_data)) == INVALID_HANDLE_VALUE) {
		return -1;
	} else {
		int err = 0;
		do {
			if (strcmp(find_data.cFileName, "..") != 0 &&
			    strcmp(find_data.cFileName, ".") != 0) {
				int added = 0;
				brrpath_infoT res = {0};
				brrpath_stat_resultT st = {0};
				st.exists = 1;
				st.type = int_determine_type(find_data.dwFileAttributes);
				/* windows gay */
				st.size = BRRTIL_WIN_ULRG(find_data.nFileSizeLow, find_data.nFileSizeHigh).QuadPart;
				/* Init 'res' */
				if ((err = int_res_fill(&res, fpath, find_data.cFileName, &st))) {
					break;
				}
				res.depth = current_depth;
				if (current_depth >= walk_options->min_depth) {
					/* Filter+add */
					if (!walk_filter || walk_filter(&res)) {
						added = 1;
						err = int_add_res(walk_result, &res);
					}
				}
				/* Recurse if dir */
				if (!err && res.type == brrpath_type_directory) {
					if (current_depth < walk_options->max_depth)
						err = int_walk_filt(res.full_path.opaque, current_depth + 1);
				}
				if (!added) {
					brrpath_info_delete(&res);
				}
			}
		} while (!err && FindNextFile(start, &find_data));
		CloseHandle(start);
		return err;
	}
}
static int BRRCALL
int_walk(const char *const fpath, brrsz base_off, const brrpath_stat_resultT *const st)
{
	brrpath_infoT res = {0};
	if (int_res_fill(&res, fpath, fpath + base_off, st)) {
		return -1;
	} else {
		res.depth = 0;
		if (res.depth >= walk_options->min_depth && res.depth < walk_options->max_depth &&
		    (!walk_filter || walk_filter(&res))) {
			if (int_add_res(walk_result, &res)) {
				brrpath_info_delete(&res);
				return -1;
			}
		} else {
			brrpath_info_delete(&res);
		}
		return int_walk_filt(fpath, 1);
	}
}
#else
static int BRRCALL
int_walk_filt(const char *const fpath, const struct stat *const sb, int type, struct FTW *ftw)
{
	if ((unsigned)ftw->level < walk_options->min_depth || (unsigned)ftw->level > walk_options->max_depth) {
		return 0;
	} else if (type == FTW_NS) {
		return -1;
		//return 0; /* TODO should this count as an error? */
	} else {
		brrpath_infoT res = {0};
		brrpath_stat_resultT st = {0};
		st.exists = 1;
		st.size = sb->st_size;
		st.type = type == FTW_F?brrpath_type_file:
			type == FTW_D?brrpath_type_directory:
			brrpath_type_other;
		if (int_res_fill(&res, fpath, fpath + ftw->base, &st))
			return -1;
		res.depth = ftw->level;
		if (!walk_filter || walk_filter(&res)) {
			/* + add res */
			if (int_add_res(walk_result, &res)) {
				brrpath_info_delete(&res);
				return -1;
			}
		} else {
			/* - del res */
			brrpath_info_delete(&res);
		}
	}
	return 0;
}
#endif

int BRRCALL brrpath_walk(
    brrpath_walk_resultT *const result,
    const brrpath_walk_optionsT *const options,
    int (*BRRCALL filter)(const brrpath_infoT *const path_info))
{
	static char start_dir[BRRPATH_MAX_PATH+1] = {0};
	brrpath_stat_resultT st = {0};
	brrsz base_off = 0;
	int error = 0;
	if (!result || !options) {
		return 0;
	} else if (!options->path.opaque) {
		return 1;
	} else if (brrpath_stat(&options->path, &st)) {
		return -1;
	} else if (!st.exists) {
		brrpath_walk_result_delete(result);
		return 0;
	}
	base_off = brrmem_search_reverse(options->path.opaque, options->path.length, BRRPATH_SEP_STR, 1, options->path.length-1);
	if (base_off == options->path.length) {
		snprintf(start_dir, BRRPATH_MAX_PATH+1, "./");
	} else if (st.type == brrpath_type_directory) {
		snprintf(start_dir, BRRPATH_MAX_PATH+1, "%s", (char *)options->path.opaque);
	} else {
		snprintf(start_dir, BRRPATH_MAX_PATH+1, "%*.*s", (int)base_off, (int)base_off, (char *)options->path.opaque);
	}
	if (!error) {
		walk_result = result;
		walk_options = options;
		walk_filter = filter;
#if defined(BRRPLATFORMTYPE_WINDOWS)
		error = int_walk(start_dir, base_off, &st);
#else
		error = nftw(start_dir, int_walk_filt, 15, FTW_MOUNT);
#endif
		walk_result = NULL;
		walk_options = NULL;
		walk_filter = NULL;
	}
	if (error) {
		brrpath_walk_result_delete(result);
	}
	return error;
}
