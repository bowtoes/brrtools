/*
Copyright 2021-2022 BowToes (bow.toes@mailfence.com)

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
#include "brrtools/brrlog.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TODO at some point, for all failing argument checks in functions should set
 * errno to EINVAL */
/* TODO also at some point, there should be such a thing as internal error
 * codes for more precise error reporting, based on the function that produces
 * the error */

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
		brrpath_components_free(&info->components);
		memset(info, 0, sizeof(*info));
	}
}
int BRRCALL
brrpath_info_get(brrpath_info_t *const info, const brrstringr_t *const path)
{
	if (!info || !path)
		return -1;

	brrpath_info_t I = {0};
	if (brrpath_stat(&I.st, path)) {
		return -1;
	}
	if (brrpath_split(&I.components, path)) {
		return -1;
	}
	if (brrpath_join(&I.full_path, &I.components)) {
		brrpath_info_free(&I);
		return -1;
	}
	if (I.exists)
		I.depth = 1;
	*info = I;
	return 0;
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
brrpath_clean(brrstringr_t *const out, const brrstringr_t *const path)
{
	if (!out || !path)
		return -1;
	int e = 0;
	brrstringr_t c = {0};
	if ((e = brrstringr_copy(&c, path))) {
		return -1;
	}
	if (brrstringr_filter_chars(&c, i_pathsep_filter)) {
		brrstringr_free(&c);
		return -1;
	}
	if (c.length > BRRPATH_MAX_PATH)
		c.length = BRRPATH_MAX_PATH;
	if (brrstringr_copy(out, &c)) {
		brrstringr_free(&c);
		return -1;
	}
	brrstringr_free(&c);
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
brrpath_split_parts(brrstringr_t *const directory, brrstringr_t *const base_name, brrstringr_t *const extension, const brrstringr_t *const path)
{
	if (!path)
		return -1;
	int e = 0;
	brrstringr_t D = {0}, B = {0}, E = {0};
	brrsz last_separator = path->length;
	brrsz last_dot = path->length;
	for(;last_separator > 0 && !i_issep(path->cstr[last_separator - 1]);--last_separator);
	for(;last_dot > last_separator && !(path->cstr[last_dot - 1] == '.');--last_dot);

	if (!e && last_separator > 0 && directory) {
		const char *const s = path->cstr;
		brrsz ln = last_separator - 1;
		e = brrstringr_new(&D, s, ln);
	}
	if (!e && last_dot > last_separator && extension) {
		const char *const s = path->cstr + last_dot;
		brrsz ln = path->length - last_dot;
		e = brrstringr_new(&E, s, ln);
	}
	if (!e && base_name) {
		const char *const s = path->cstr + last_separator;
		brrsz ln = path->length - last_separator;
		if (last_dot != last_separator && extension)
			ln = last_dot - last_separator - 1;
		e = brrstringr_new(&B, s, ln);
	}

	if (e) {
		brrstringr_free(&D);
		brrstringr_free(&B);
		brrstringr_free(&E);
	} else {
		if (directory)
			*directory = D;
		if (base_name)
			*base_name = B;
		if (extension)
			*extension = E;
	}
	return e;
}
int BRRCALL
brrpath_split(brrpath_components_t *const out, const brrstringr_t *const path)
{
	if (!path || !out)
		return -1;
	return brrpath_split_parts(&out->directory, &out->base_name, &out->extension, path);
}
int BRRCALL
brrpath_join_parts(brrstringr_t *const out,
	const brrstringr_t *const directory, const brrstringr_t *const base_name, const brrstringr_t *const extension)
{
	if (!out)
		return -1;
	int e = 0;
	brrstringr_t s = {0};
	if (!e && directory) {
		if (BRRSZ_MAX == brrstringr_print(&s, s.length, BRRPATH_MAX_PATH - s.length, "%s%c", directory->cstr, BRRPATH_SEP_CHR)) {
			e = -1;
		}
	}
	if (!e && base_name) {
		if (BRRSZ_MAX == brrstringr_print(&s, s.length, BRRPATH_MAX_PATH - s.length, "%s", base_name->cstr)) {
			e = -1;
		}
	}
	if (!e && extension) {
		if (BRRSZ_MAX == brrstringr_print(&s, s.length, BRRPATH_MAX_PATH - s.length, ".%s", extension->cstr)) {
			e = -1;
		}
	}
	if (e) {
		brrstringr_free(&s);
		return -1;
	}
	*out = s;
	return e;
}
int BRRCALL
brrpath_join(brrstringr_t *const out, const brrpath_components_t *const components)
{
	if (!out || !components)
		return -1;

	brrstringr_t s = {0};
	int err = 0;
	return brrpath_join_parts(out,
		components->directory.cstr?&components->directory:NULL,
		components->base_name.cstr?&components->base_name:NULL,
		components->extension.cstr?&components->extension:NULL);
}

int BRRCALL
brrpath_directory(brrstringr_t *const out, const brrstringr_t *const path)
{
	/* TODO TEST */
	if (!path || !out)
		return -1;

	brrstringr_t s = {0};
	brrsz last_sep = path->length;
	for (;last_sep > 0 && !i_issep(path->cstr[last_sep - 1]);--last_sep);

	const char *const S = path->cstr;
	brrsz ln = last_sep;
	if (brrstringr_new(&s, S, ln))
		return -1;

	*out = s;
	return 0;
}
int BRRCALL
brrpath_base_name(brrstringr_t *const out, const brrstringr_t *const path)
{
	/* TODO TEST */
	if (!path || !out)
		return -1;

	brrstringr_t s = {0};
	brrsz last_sep = path->length;
	brrsz last_dot = path->length;
	for (;last_sep > 0 && !i_issep(path->cstr[last_sep - 1]);--last_sep);
	for (;last_dot > last_sep && !(path->cstr[last_dot - 1] == '.');--last_dot);

	const char *const S = path->cstr + last_sep;
	brrsz ln = path->length - last_sep;
	if (last_dot > last_sep)
		ln = last_dot - last_sep - 1;
	if (brrstringr_new(&s, S, ln))
		return -1;
	*out = s;
	return 0;
}
int BRRCALL
brrpath_extension(brrstringr_t *const out, const brrstringr_t *const path)
{
	/* TODO TEST */
	if (!out || !path)
		return -1;

	brrstringr_t s = {0};
	brrsz last_sep = path->length;
	brrsz last_dot = path->length;
	for (;last_sep > 0 && !i_issep(path->cstr[last_sep - 1]);--last_sep);
	for (;last_dot > last_sep && !(path->cstr[last_dot - 1] == '.');--last_dot);
	const char *S;
	brrsz ln;
	if (last_dot == last_sep) {
		S = path->cstr + path->length;
		ln = 0;
	} else {
		S = path->cstr + last_dot - 1;
		ln = path->length - last_dot + 1;
	}
	if (brrstringr_new(&s, S, ln))
		return -1;
	*out = s;
	return 0;
}

static brrpath_walk_result_t *walk_result;
static const brrpath_walk_options_t *walk_options;
#if defined(BRRPLATFORMTYPE_Windows)
/* TODO minimize 'i_walk' and 'i_walk_filt' to be a single function? */
/* TODO WINDOWS IS UNTESTED DEAR GOD */
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
				if ((err = brrpath_join(&res.full_path, &res.components))) {
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
	if (brrpath_join(&res.full_path, &res.components)) {
		return -1;
	} else if (brrpath_split(&res.components, &res.full_path)) {
		brrpath_info_free(&res);
		return -1;
	}
	res.exists = st->exists;
	res.type = st->type;
	res.size = st->size;
	res.depth = 0;
	if (res.depth >= walk_options->min_depth && res.depth <= walk_options->max_depth &&
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
#elif defined(BRRPLATFORMTYPE_unix)
static int BRRCALL
i_walk_filt(const char *const fpath, const struct stat *const sb, int type, struct FTW *ftw)
{
	if ((unsigned)ftw->level < walk_options->min_depth) {
		return FTW_SKIP_SIBLINGS;
	} else if ((unsigned)ftw->level > walk_options->max_depth) {
		return FTW_SKIP_SUBTREE;
	} else if (type == FTW_NS) {
		return FTW_STOP;
		//return 0; /* TODO should this count as an error? */
	} else {
		brrpath_info_t res = {0};
		switch (type) {
			case FTW_F: res.type = brrpath_type_file; break;
			case FTW_D: res.type = brrpath_type_directory; break;
			default: res.type = brrpath_type_other; break;
		}
		res.exists = 1;
		res.size = sb->st_size;
		res.depth = ftw->level;
		brrstringr_t ffpath = brrstringr_cast(fpath);
		if (brrpath_split(&res.components, &ffpath)) {
			return FTW_STOP;
		}
		if (brrpath_join(&res.full_path, &res.components)) {
			brrpath_info_free(&res);
			return FTW_STOP;
		}
		if (!walk_options->filter || !walk_options->filter(&res)) {
			if (brrheap_append((void**)&walk_result->results,
			        &walk_result->n_results, sizeof(res), &res)) {
				brrpath_info_free(&res);
				return FTW_STOP;
			}
		} else {
			brrpath_info_free(&res);
		}
	}
	return FTW_CONTINUE;
}
#endif
int BRRCALL
brrpath_walk(brrpath_walk_result_t *const result,
    const brrstringr_t *const path, brrpath_walk_options_t options)
{
	int error = 0;
	if (!result || !path)
		return -1;

	brrpath_stat_result_t st = {0};
	if (brrpath_stat(&st, path))
		return -1;
	if (st.exists) {
		brrpath_walk_result_t rs = {0};
		brrstringr_t dir = {0};

		if (st.type == brrpath_type_directory) {
			dir = *path;
		} else {
			/* Path is not a directory */
			if (brrpath_directory(&dir, path)) {
				return -1;
			}
			if (dir.length == 0) {
				/* Path is a file local to the current directory */
				if (brrstringr_copy(&dir, &(brrstringr_t)brrstringr_literal("."))) {
					brrstringr_free(&dir);
					return -1;
				}
			}
		}
		walk_result = &rs;
		walk_options = &options;
		#if defined(BRRPLATFORMTYPE_Windows)
		error = i_walk(dir.cstr, &st);
		#else
		error = nftw(dir.cstr, i_walk_filt, 15, FTW_MOUNT | FTW_ACTIONRETVAL);
		#endif
		walk_result = NULL;
		walk_options = NULL;
		if (error < 0) {
			brrpath_walk_result_free(&rs);
			error = -1;
		}
		if (st.type != brrpath_type_directory)
			brrstringr_free(&dir);
		if (error >= 0)
			*result = rs;
	}
	return -(error < 0) ;
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
