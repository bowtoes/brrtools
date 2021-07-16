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
#include "brrtools/brrlog.h"
#include "brrtools/brrmem.h"
#include "brrtools/brrtil.h"

void BRRCALL
brrpath_info_delete(brrpath_infoT *const info)
{
	if (info) {
		brrstg_delete(&info->full_path);
		brrstg_delete(&info->directory);
		brrstg_delete(&info->base_name);
		brrstg_delete(&info->extension);
	}
}
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
				error = 0;
				*st = (brrpath_stat_resultT){0};
			}
		} else {
			ULARGE_INTEGER i = {0};
			st->exists = 1;
			st->type = int_determine_type(attr.dwFileAttributes);
			i.HighPart = attr.nFileSizeHigh;
			i.LowPart = attr.nFileSizeLow;
			st->size = i.QuadPart;
		}
#else
		struct stat s = {0};
		if (stat(path->opaque, &s)) {
			error = errno;
			if (error == ENOENT) {
				error = 0;
				*st = (brrpath_stat_resultT){0};
			}
		} else {
			st->exists = 1;
			st->size = s.st_size;
			st->type = int_determine_type(s.st_mode);
		}
#endif
	}
	return error;
}

static brrpath_walk_resultT *walk_result;
static const brrpath_walk_optionsT *walk_options;
static int (*BRRCALL walk_filter)(const brrpath_infoT *const path_info);

#if defined(BRRPLATFORMTYPE_WINDOWS)
# define T_IS_DIR(_type_) ((_type_) & FILE_ATTRIBUTE_DIRECTORY)
# define T_IS_REG(_type_) ((_type_) & FILE_ATTRIBUTE_NORMAL)
#else
# define T_IS_DIR(_type_) ((_type_) == FTW_D)
# define T_IS_REG(_type_) ((_type_) == FTW_F)
#endif
#if defined(BRRPLATFORMTYPE_WINDOWS)
static int BRRCALL
int_res_fill(brrpath_infoT *const res, const char *const fpath, const WIN32_FIND_DATA *const data, int exists)
{
	int err = 0;
	if (!(err = brrstg_new(&res->directory, fpath, BRRPATH_MAX_PATH))) {
		if (!(err = brrstg_new(&res->base_name, data->cFileName, BRRPATH_MAX_NAME))) {
			if (!(err = brrstg_new(&res->full_path, NULL, 0))) {
				if (!(err = brrstg_print(&res->full_path, BRRPATH_MAX_PATH, 0, NULL, "%s\\%s", fpath, data->cFileName))) {
					if (T_IS_DIR(data->dwFileAttributes)) {
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
	}
	if (err) {
		brrpath_info_delete(res);
	} else {
		res->exists = exists;
		res->type = !data->dwFileAttributes?brrpath_type_none:
		    T_IS_DIR(data->dwFileAttributes)?brrpath_type_directory:
		    T_IS_REG(data->dwFileAttributes)?brrpath_type_file:brrpath_type_other;
		if (res->type != brrpath_type_directory) {
			ULARGE_INTEGER s = {0};
			s.HighPart = data->nFileSizeHigh;
			s.LowPart = data->nFileSizeLow;
			res->size = s.QuadPart;
		}
	}
	return err;
}
#else
static int BRRCALL
int_res_fill(brrpath_infoT *const res, const char *const fpath, brru8 size, unsigned int type, int base_ofs, int exists)
{
	int err = 0;
	if (!(err = brrstg_new(&res->full_path, fpath, BRRPATH_MAX_PATH))) {
		if (!(err = brrstg_new(&res->directory, fpath, base_ofs - 1))) {
			if (!(err = brrstg_new(&res->base_name, fpath + base_ofs, BRRPATH_MAX_NAME))) {
				if (T_IS_DIR(type)) {
					err = brrstg_new(&res->extension, NULL, 0);
				} else {
					brrsz end = brrmem_search_reverse(res->base_name.opaque,
					    res->base_name.length, ".", 1, res->base_name.length - 1);
					if (end && end != res->base_name.length)
						err = brrstg_new(&res->extension, fpath + base_ofs + end + 1, BRRPATH_MAX_NAME - end);
					else
						err = brrstg_new(&res->extension, NULL, 0);
				}
			}
		}
	}
	if (err) {
		brrpath_info_delete(res);
	} else {
		res->exists = exists;
		res->type = !exists?brrpath_type_none:
		    T_IS_DIR(type)?brrpath_type_directory:T_IS_REG(type)?brrpath_type_file:brrpath_type_other;
		if (res->type != brrpath_type_directory)
			res->size = size;
	}
	return err;
}
#endif
static int BRRCALL
int_add_res(const brrpath_infoT *const res)
{
	if (brrlib_alloc((void **)&walk_result->walk_results,
	    (walk_result->result_count + 1) * sizeof(brrpath_infoT), 0)) {
		return -1;
	}
	walk_result->walk_results[walk_result->result_count] = *res;
	walk_result->result_count++;
	return 0;
}
#if defined(BRRPLATFORMTYPE_WINDOWS)
static int BRRCALL
int_walk(const char *fpath, brrsz current_depth)
{
	HANDLE start;
	WIN32_FIND_DATA find_data;
	char path[BRRPATH_MAX_PATH+1] = {0};
	/* TODO the depth seems to go one too far on windows specifically. */
	if (current_depth >= walk_options->max_depth)
		return 0;
	snprintf(path, BRRPATH_MAX_PATH+1, "%s\\*.*", fpath);
	if ((start = FindFirstFile(path, &find_data)) == INVALID_HANDLE_VALUE) {
		BRRLOG_ERROR("FIND FIRST FILE %d", GetLastError());
		return -1;
	} else {
		int err = 0;
		do {
			if (strcmp(find_data.cFileName, "..") != 0 &&
			    strcmp(find_data.cFileName, ".") != 0) {
				int added = 0;
				brrpath_infoT res = {0};
				/* Init 'res' */
				if ((err = int_res_fill(&res, fpath, &find_data, 1))) {
					break;
				}
				res.depth = current_depth;
				if (current_depth >= walk_options->min_depth) {
					/* Filter+add */
					if (!walk_filter || walk_filter(&res)) {
						added = 1;
						err = int_add_res(&res);
					}
				}
				/* Recurse if dir */
				if (!err && res.type == brrpath_type_directory) {
					err = int_walk(res.full_path.opaque, current_depth + 1);
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
#else
static int BRRCALL
int_walk_filt(const char *const fpath, const struct stat *const sb, int type, struct FTW *ftw)
{
	if ((unsigned)ftw->level < walk_options->min_depth || (unsigned)ftw->level >= walk_options->max_depth) {
		return 0;
	} else if (type == FTW_NS) {
		return -1;
		//return 0; /* TODO should this count as an error? */
	} else {
		brrpath_infoT res = {0};
		if (int_res_fill(&res, fpath, sb->st_size, type, ftw->base, 1)) {
			return -1;
		}
		res.depth = ftw->level;
		if (!walk_filter || walk_filter(&res)) {
			/* + add res */
			if (int_add_res(&res)) {
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
	brrstgT start_dir = {0};
	brrpath_stat_resultT st = {0};
	int error = 0;
	if (!result || !options) {
		BRRLOG_NOR("a");
		return 0;
	} else if (!options->path.opaque) {
		BRRLOG_NOR("b");
		return 1;
	} else if (brrpath_stat(&options->path, &st)) {
		BRRLOG_NOR("c");
		return -1;
	} else if (!st.exists) {
		BRRLOG_NOR("d");
		brrpath_walk_result_delete(result);
		return 0;
	} else if (st.type == brrpath_type_directory) {
		error = brrstg_copy(&start_dir, &options->path);
	} else {
		brrsz end = brrmem_search_reverse(&options->path, options->path.length, BRRPATH_SEP_STR, 1, options->path.length);
		error = brrstg_range(&start_dir, &options->path, 0, end, 0, 0);
	}
	BRRLOG_NOR("e");
	if (!error) {
		BRRLOG_NOR("e.1");
		walk_result = result;
		walk_options = options;
		walk_filter = filter;
#if defined(BRRPLATFORMTYPE_WINDOWS)
		error = int_walk(start_dir.opaque, 0);
#else
		error = nftw(start_dir.opaque, int_walk_filt, 15, FTW_MOUNT);
#endif
		brrstg_delete(&start_dir);
		walk_result = NULL;
		walk_options = NULL;
		walk_filter = NULL;
	}
	if (error) {
		brrpath_walk_result_delete(result);
	}
	return error;
}
