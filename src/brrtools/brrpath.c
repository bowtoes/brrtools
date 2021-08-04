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
brrpath_stat(brrpath_stat_resultT *const st, const char *const path)
{
	int error = 0;
	if (!path || !st) {
		return 0;
	} else {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		WIN32_FILE_ATTRIBUTE_DATA attr = {0};
		if (!GetFileAttributesEx(path, GetFileExInfoStandard, &attr)) {
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
		if (stat(path, &s)) {
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
void BRRCALL
brrpath_info_delete(brrpath_infoT *const info)
{
	if (info) {
		brrstg_delete(&info->full_path);
		brrstg_delete(&info->components.directory);
		brrstg_delete(&info->components.base_name);
		brrstg_delete(&info->components.extension);
		info->depth = info->exists = info->size = info->type = 0;
	}
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
/* remove duplicate separators & invalid characters */
static int BRRCALL
int_path_clean(brrstgT *const path)
{
	char *p = (char *)path->opaque;
	brrsz nl = 0;
	for (brrsz i = 0; i < path->length; ++i) {
		if (!int_char_is_sep(p[i]) || (i + 1 <= path->length && !int_char_is_sep(p[i+1]))) {
			if (int_char_is_valid_path(p[i])) {
				p[nl++] = p[i];
			}
		}
	}
	return brrsizestr(path, nl)?0:-1;
}
int BRRCALL
brrpath_combine(brrstgT *const string, const char *const directory,
    const char *const base_name, const char *const extension)
{
	static char tmp[BRRPATH_MAX_PATH + 1] = {0};
	brrsz of = 0;
	int e = 0;
	if (!string)
		return 0;
	if (directory)
		of += snprintf(tmp + of, BRRPATH_MAX_PATH + 1 - of, "%s"BRRPATH_SEP_STR, directory);
	if (base_name)
		of += snprintf(tmp + of, BRRPATH_MAX_PATH + 1 - of, "%s", base_name);
	if (extension)
		of += snprintf(tmp + of, BRRPATH_MAX_PATH + 1 - of, ".%s", extension);
	if (!(e = brrstg_new(string, tmp, of))) {
		e = int_path_clean(string);
	}
	return e;
}
int BRRCALL
brrpath_split(brrstgT *const directory, brrstgT *const base_name,
    brrstgT *const extension, const char *const path)
{
	int e = 0;
	brrstgT m = {0};
	brrsz last_sep = 0;
	brrsz last_dot = 0;
	int did_have_sep = 0;
	if (!path)
		return 0;
	if (!(e = brrstg_new(&m, path, BRRPATH_MAX_NAME))) {
		did_have_sep = int_char_is_sep(path[m.length]);
		e = int_path_clean(&m);
	}
	if (!e) {
		if (m.length == 0) {
			brrstg_delete(directory);
			e = brrstg_new(base_name, NULL, 0);
			brrstg_delete(extension);
		} else {
			last_sep = brrmem_search_reverse(m.opaque, m.length, BRRPATH_SEP_STR, 1, m.length - 1);
			last_dot = brrmem_search_reverse(m.opaque, m.length, ".", 1, m.length - 1);
			if (last_sep == m.length) {
				if (last_dot == m.length) {
					brrstg_delete(extension);
					e = brrstg_copy(base_name, &m);
				} else {
					if (!(e = brrstg_new(extension, (char *)m.opaque + last_dot + 1, m.length - last_dot - 1))) {
						e = brrstg_new(base_name, (char *)m.opaque, last_dot);
					}
				}
				brrstg_delete(directory);
			} else {
				if (last_dot < last_sep || last_dot == m.length) {
					brrstg_delete(extension);
					e = brrstg_new(base_name, (char *)m.opaque + last_sep + 1, m.length - last_sep);
				} else {
					if (!(e = brrstg_new(extension, (char *)m.opaque + last_dot + 1, m.length - last_dot - 1))) {
						e = brrstg_new(base_name, (char *)m.opaque + last_sep + 1, last_dot - last_sep - 1);
					}
				}
				if (!e) {
					e = brrstg_new(directory, m.opaque, last_sep);
				}
			}
		}
	}
	brrstg_delete(&m);
	if (e) {
		brrstg_delete(directory);
		brrstg_delete(base_name);
		brrstg_delete(extension);
		return -1;
	}
	return 0;
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
/* TODO minimize 'int_walk' and 'int_walk_filt' to be a single function? */
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
				if ((err = brrpath_combine(&res.full_path, fpath, find_data.cFileName, NULL))) {
					brrpath_info_delete(&res);
					break;
				}
				if ((err = brrpath_split(&res.components.directory,
				    &res.components.base_name, &res.components.extension, res.full_path.opaque))) {
					brrpath_info_delete(&res);
					break;
				}
				res.exists = st.exists;
				res.type = st.type;
				res.size = st.size;
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
int_walk(const char *const fpath, const brrpath_stat_resultT *const st)
{
	brrpath_infoT res = {0};
	if (brrpath_combine(&res.full_path, fpath, NULL, NULL)) {
		return -1;
	} else if (brrpath_split(&res.components.directory,
	    &res.components.base_name, &res.components.extension, res.full_path.opaque)) {
		brrpath_info_delete(&res);
		return -1;
	}
	res.exists = st->exists;
	res.type = st->type;
	res.size = st->size;
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
		res.exists = 1;
		res.size = sb->st_size;
		res.type = type == FTW_F?brrpath_type_file:
			type == FTW_D?brrpath_type_directory:
			brrpath_type_other;
		res.depth = ftw->level;
		if (brrpath_split(&res.components.directory,
		    &res.components.base_name, &res.components.extension, fpath)) {
			return -1;
		} else if (brrpath_combine(&res.full_path,
		    res.components.directory.opaque,
		    res.components.base_name.opaque,
		    res.components.extension.opaque)) {
			brrpath_info_delete(&res);
			return -1;
		} else if (!walk_filter || walk_filter(&res)) {
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
int BRRCALL brrpath_walk(const char *const path,
    brrpath_walk_resultT *const result,
    const brrpath_walk_optionsT *const options,
    int (*BRRCALL filter)(const brrpath_infoT *const path_info))
{
	brrpath_componentsT comps = {0};
	brrpath_stat_resultT st = {0};
	int error = 0;
	if (!result || !options || !path) {
		return 0;
	} else if (brrpath_stat(&st, path)) {
		brrpath_walk_result_delete(result);
		return -1;
	} else if (!st.exists) {
		brrpath_walk_result_delete(result);
		return 0;
	} else if (!(error = brrpath_split(&comps.directory, &comps.base_name, &comps.extension, path))) {
		brrstgT full = {0};
		brrstgT start_dir = {0};
		if (!(error = brrpath_combine(&full, comps.directory.opaque, comps.base_name.opaque, comps.extension.opaque))) {
			if (st.type == brrpath_type_directory) {
				error = brrpath_combine(&start_dir, full.opaque, NULL, NULL);
			} else {
				error = brrpath_combine(&start_dir, BRRTIL_SAFESTR(comps.directory.opaque, "."), NULL, NULL);
			}
		}
		if (!error) {
			walk_result = result;
			walk_options = options;
			walk_filter = filter;
#if defined(BRRPLATFORMTYPE_WINDOWS)
			error = int_walk(start_dir.opaque, &st);
#else
			error = nftw(start_dir.opaque, int_walk_filt, 15, FTW_MOUNT);
#endif
			walk_result = NULL;
			walk_options = NULL;
			walk_filter = NULL;
		}
		brrstg_delete(&full);
		brrstg_delete(&start_dir);
	}
	if (error) {
		brrpath_walk_result_delete(result);
	}
	brrstg_delete(&comps.directory);
	brrstg_delete(&comps.base_name);
	brrstg_delete(&comps.extension);
	return error;
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
