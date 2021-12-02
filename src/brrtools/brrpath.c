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
#include <stdlib.h>
#include <string.h>

#if defined(BRRPLATFORMTYPE_Windows)
#elif defined(BRRPLATFORMTYPE_unix)
/* FIXME need to figure out which nixes do/don't have <ftw.h> */
# include <ftw.h>
#else
# error Unknown system
#endif

#if defined(BRRPLATFORMTYPE_unix)
/* FIXME *nix universal? */
# include <sys/stat.h>
#endif

#include "brrtools/brrlib.h"
#include "brrtools/brrheap.h"

static brrpath_type_t BRRCALL
i_determine_type(brru8 attr)
{
	brrpath_type_t type = brrpath_type_none;
#if defined(BRRPLATFORMTYPE_Windows)
	type = (attr & FILE_ATTRIBUTE_DIRECTORY)?brrpath_type_directory:
	       (attr & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_VIRTUAL))?brrpath_type_other:
	       brrpath_type_file;
#else
	type = S_ISDIR(attr)?brrpath_type_directory:
	       S_ISREG(attr)?brrpath_type_file:
	       brrpath_type_other;
#endif
	return type;
}
int BRRCALL
brrpath_stat(brrpath_stat_result_t *const st, const brrstringr_t *const path)
{
	int error = 0;
	if (!path || !st) {
		return -1;
	} else {
#if defined(BRRPLATFORMTYPE_Windows)
		WIN32_FILE_ATTRIBUTE_DATA attr = {0};
		if (!GetFileAttributesEx(path->cstr, GetFileExInfoStandard, &attr)) {
			error = GetLastError();
			if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
				/* Path doesn't exist. */
				error = 0;
			} else {
				error = -1;
			}
			*st = (brrpath_stat_result_t){0};
		} else {
			st->exists = 1;
			st->type = i_determine_type(attr.dwFileAttributes);
			st->size = _brrpath_ulrg(attr.nFileSizeLow, attr.nFileSizeHigh);
		}
#else
		struct stat s = {0};
		if (stat(path->cstr, &s)) {
			error = errno;
			if (error == ENOENT) {
				/* Path doesn't exist, no error. */
				error = 0;
			} else {
				error = -1;
			}
			*st = (brrpath_stat_result_t){0};
		} else {
			st->exists = 1;
			st->size = s.st_size;
			st->type = i_determine_type(s.st_mode);
		}
#endif
	}
	return error;
}
void BRRCALL
brrpath_info_free(brrpath_info_t *const info)
{
	if (info) {
		brrstringr_free(&info->full_path);
		brrstringr_free(&info->components.directory);
		brrstringr_free(&info->components.base_name);
		brrstringr_free(&info->components.extension);
		memset(info, 0, sizeof(*info));
	}
}

static brrbl BRRCALL
i_issep(char ch)
{
#if defined(BRRPLATFORMTYPE_Windows)
	return ch == '/' || ch == '\\';
#else
	return ch == '/';
#endif
}
static brrbl BRRCALL
i_char_is_valid_path(char ch)
{
#if defined(BRRPLATFORMTYPE_Windows)
	return ch != '<' && ch != '>' &&
	       ch != ':' && ch != '"' &&
	       ch != '|' && ch != '?' &&
	       ch != '*' && ch >= ' ';
#else
	return ch != 0;
#endif
}
/* Filters out consecutive separators/invalid characters */
static int BRRCALL
i_pathsep_filter(int current, int next)
{
	if (!i_issep(current) || !i_issep(next)) {
		if (i_char_is_valid_path(current))
			return 0;
	}
	return 1;
}

int BRRCALL
brrpath_split(brrpath_components_t *const components, const brrstringr_t *const path)
{
	int e = 0;
	brrbl did_have_sep = 0;
	brrstringr_t clean = {0};
	brrpath_components_t c = {0};
	if (!path || !components)
		return -1;
	if (!(e = brrstringr_new(&clean, path->cstr, brrstringr_length(path->cstr, BRRPATH_MAX_PATH)))) {
		if (brrstringr_filter_chars(&clean, i_pathsep_filter)) {
			brrstringr_free(&clean);
			return -1;
		}
		did_have_sep = i_issep(path->cstr[path->length]);
	}
	/* TODO this is very ugly in terms of layout */
	if (clean.length) {
		brrsz last_sep = clean.length;
		brrsz last_dot = clean.length;
		for (;last_sep > 0 && !i_issep(clean.cstr[last_sep - 1]); --last_sep);
		for (;last_dot > 0 && clean.cstr[last_dot - 1] != '.'; --last_dot);
		if (!last_sep) {
			/* clean contains no separators, no directory */
			if (!last_dot) {
				/* clean contains no dot, no extension */
				e = brrstringr_copy(&c.base_name, &clean);
			} else {
				/* clean contains a dot, extension */
				e = brrstringr_new(&c.base_name, clean.cstr, last_dot);
				if (!e)
					e = brrstringr_new(&c.extension, clean.cstr + last_dot + 1, clean.length - last_dot - 1);
			}
		} else {
			/* clean contains a separator, directory */
			if (last_dot < last_sep || last_dot == clean.length) {
				/* clean does not contain a dot in the base name, no extension */
				e = brrstringr_new(&c.base_name, clean.cstr + last_sep + 1, clean.length - last_sep);
			} else {
				/* clean contains a dot in the base name, extension */
				e = brrstringr_new(&c.base_name, clean.cstr + last_sep + 1, last_dot - last_sep - 1);
				if (!e)
					e = brrstringr_new(&c.extension, clean.cstr + last_dot + 1, clean.length - last_dot - 1);
			}
			if (!e)
				e = brrstringr_new(&c.directory, clean.cstr, last_sep);
		}
	}

	brrstringr_free(&clean);
	if (e) {
		brrpath_components_free(&c);
		return -1;
	}
	*components = c;
	return 0;
}
int BRRCALL
brrpath_combine(const brrpath_components_t *const components, brrstringr_t *const path)
{
	brrstringr_t s = {0};
	int err = 0;
	if (!path || !components)
		return -1;
	if (!err && components->directory.cstr) {
		if (brrstringr_print(&s, s.length, BRRPATH_MAX_PATH - s.length, "%s%c", components->directory.cstr, BRRPATH_SEP_CHR))
			err = -1;
	}
	if (!err && components->base_name.cstr) {
		if (brrstringr_print(&s, s.length, BRRPATH_MAX_PATH - s.length, "%s", components->base_name.cstr))
			err = -1;
	}
	if (!err && components->extension.cstr) {
		if (brrstringr_print(&s, s.length, BRRPATH_MAX_PATH - s.length, ".%s", components->extension.cstr))
			err = -1;
	}
	if (err) {
		brrstringr_free(&s);
		return -1;
	}
	*path = s;
	return 0;
}
void BRRCALL
brrpath_components_free(brrpath_components_t *const components)
{
	if (components) {
		brrstringr_free(&components->directory);
		brrstringr_free(&components->base_name);
		brrstringr_free(&components->extension);
		memset(components, 0, sizeof(*components));
	}
}

int BRRCALL
brrpath_extract_directory(brrstringr_t *const string, const brrstringr_t *const path)
{
	if (!string || !path)
		return -1;
	brrsz e = path->length;
	for (;e > 0 && !i_issep(path->cstr[e - 1]); --e);
	if (brrstringr_new(string, path->cstr, e))
		return -1;
	return 0;
}

static brrpath_walk_result_t *walk_result;
static const brrpath_walk_options_t *walk_options;
#if defined(BRRPLATFORMTYPE_Windows)
/* TODO minimize 'i_walk' and 'i_walk_filt' to be a single function? */
static int BRRCALL
i_walk_filt(const char *const fpath, brrsz current_depth)
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
				brrpath_info_t res = {.components = {
					.directory = brrstringr_cast(fpath),
					.base_name = brrstringr_cast(find_data.cFileName),
					.extension = (brrstringr_t){0},
				}};
				brrpath_stat_result_t st = {0};
				st.exists = 1;
				st.type = i_determine_type(find_data.dwFileAttributes);
				st.size = _brrpath_ulrg(find_data.nFileSizeLow, find_data.nFileSizeHigh);
				/* Init 'res' */
				if ((err = brrpath_combine(&res.components, &res.full_path))) {
					break;
				} else if ((err = brrpath_split(&res.components, &res.full_path))) {
					brrpath_info_free(&res);
					break;
				}
				res.exists = st.exists;
				res.type = st.type;
				res.size = st.size;
				res.depth = current_depth;
				if (current_depth >= walk_options->min_depth) {
					/* Filter+add */
					if (!walk_options->filter || !walk_options->filter(&res)) {
						added = 1;
						err = brrheap_append((void **)&walk_result->results,
						    &walk_result->n_results, sizeof(res), &res);
					}
				}
				/* Recurse if dir */
				if (!err && res.type == brrpath_type_directory) {
					if (current_depth < walk_options->max_depth)
						err = i_walk_filt(res.full_path.cstr, current_depth + 1);
				}
				if (!added) /* Theres probably a better way to do this? */
					brrpath_info_free(&res);
			}
		} while (!err && FindNextFile(start, &find_data));
		CloseHandle(start);
		return err;
	}
}
/* Entrance of the walk */
static int BRRCALL
i_walk(const char *const fpath, const brrpath_stat_result_t *const st)
{
	brrpath_info_t res = {.components = {
		.directory = brrstringr_cast(fpath),
	}};
	if (brrpath_combine(&res.components, &res.full_path)) {
		return -1;
	} else if (brrpath_split(&res.components, &res.full_path)) {
		brrpath_info_free(&res);
		return -1;
	}
	res.exists = st->exists;
	res.type = st->type;
	res.size = st->size;
	res.depth = 0;
	if (res.depth >= walk_options->min_depth && res.depth < walk_options->max_depth &&
	   (!walk_options->filter || !walk_options->filter(&res))) {
		if (brrheap_append((void**)&walk_result->results,
		        &walk_result->n_results, sizeof(res), &res)) {
			brrpath_info_free(&res);
			return -1;
		}
	} else {
		brrpath_info_free(&res);
	}
	return i_walk_filt(fpath, 1);

}
#else
static int BRRCALL
i_walk_filt(const char *const fpath, const struct stat *const sb, int type, struct FTW *ftw)
{
	if ((unsigned)ftw->level < walk_options->min_depth || (unsigned)ftw->level > walk_options->max_depth) {
		return 0;
	} else if (type == FTW_NS) {
		return -1;
		//return 0; /* TODO should this count as an error? */
	} else {
		brrstringr_t ffpath = brrstringr_cast(fpath);
		brrpath_info_t res = {0};
		res.exists = 1;
		res.size = sb->st_size;
		res.type = type == FTW_F?brrpath_type_file:
			type == FTW_D?brrpath_type_directory:
			brrpath_type_other;
		res.depth = ftw->level;
		if (brrpath_split(&res.components, &ffpath)) {
			return -1;
		} else if (brrpath_combine(&res.components, &res.full_path)) {
			brrpath_info_free(&res);
			return -1;
		} else if (!walk_options->filter || walk_options->filter(&res)) {
			if (brrheap_append((void**)&walk_result->results,
			        &walk_result->n_results, sizeof(res), &res)) {
				brrpath_info_free(&res);
				return -1;
			}
		} else {
			brrpath_info_free(&res);
		}
	}
	return 0;
}
#endif
int BRRCALL brrpath_walk(brrpath_walk_result_t *const result,
    const brrstringr_t *const path, brrpath_walk_options_t options)
{
	brrpath_stat_result_t st = {0};
	brrpath_walk_result_t rs = {0};
	int error = 0;
	if (!result || !path)
		return -1;
	if (brrpath_stat(&st, path))
		return -1;
	if (st.exists) {
		brrstringr_t start_dir = {0};
		if (brrpath_extract_directory(&start_dir, path))
			return -1;
		walk_result = &rs;
		walk_options = &options;
		#if defined(BRRPLATFORMTYPE_Windows)
		error = i_walk(start_dir.cstr, &st);
		#else
		error = nftw(start_dir.cstr, i_walk_filt, 15, FTW_MOUNT);
		#endif
		brrstringr_free(&start_dir);
		walk_result = NULL;
		walk_options = NULL;
		if (error) {
			brrpath_walk_result_free(&rs);
			return -1;
		}
	}
	*result = rs;
	return 0;
}
void BRRCALL
brrpath_walk_result_free(brrpath_walk_result_t *const result)
{
	if (result) {
		brrheap_clear((void**)&result->results, &result->n_results, sizeof(*result->results),
		    (void(*)(void*))brrpath_info_free);
		memset(result, 0, sizeof(*result));
	}
}
