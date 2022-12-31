/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include "brrtools/brrpath.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#if brrtools_debug
# include <stdio.h>
#endif
#if brrplat_unix
# include <sys/stat.h>
# include <ftw.h>
# include <unistd.h>
#elif brrplat_dos
# include <aclapi.h>
# define _win_ulrg(_hi_, _lo_) (((ULARGE_INTEGER){.u={.LowPart=(_lo_),.HighPart=(_hi_)}}).QuadPart)
#endif

#include "brrmacro.h"
#include "_util.h"

typedef struct i_pfx {
	const char *s;
	brrsz l;

	union {
		brrpath_inf;
		brrpath_inf_t f;
	};

	#define pfx_none 0
	#define pfx_drive 1
	#define pfx_unc 2
	#define pfx_namespace 3
	#define pfx_global 4
	int pfx_type;
} i_pfx_t;

#if brrplat_unix
#define i_get_pfx_type(_p_, _l_) ((i_pfx_t){.s=NULL, .l=0})
#elif brrplat_dos
static inline i_pfx_t BRRCALL i_get_pfx_type(const char *const p, brrsz l) {
	#define i_caseless(_c_, _x_) (((_c_) == (_x_)) || ((_c_) == (_x_) + ('a' - 'A')))
	#define i_pfx(_s_, ...) ((i_pfx_t){.s=(_s_), .l=sizeof(_s_) - 1, __VA_ARGS__})

	if (l >= 2) {
		if ((p[0] >= 'A' && p[0] <= 'Z') || (p[0] >= 'a' && p[0] <= 'z')) {
			/* Path starts with an ascii character */
			if (p[1] == ':') {
				/* Path looks like '[A-Za-z]:*' */
				return i_pfx("A:", .type = pfx_drive, .abs = l > 2 && brrpath_issep(p[2]));
			}
			/* Path is something else */
		} else if (brrpath_issep(p[0])) {
			if (brrpath_issep(p[1])) {
				/* Path looks like '\\*' */
				if (brrpath_issep(p[3])) {
					if (p[2] == '?') /* Path looks like '\\?\*' */
						return i_pfx("\\\\?\\", .type = pfx_namespace, .abs=1);
					else if (p[2] == '.') /* Path looks like '\\.\*' */
						return i_pfx("\\\\.\\", .type = pfx_namespace, .abs=1);
				}
				return i_pfx("\\\\", .type = pfx_unc, .abs=1);
			} else {
				/* Path looks like '\*' */
				if (l >= sizeof("\\Global??\\") - 1) {
					if (i_caseless(p[1], 'G') && i_caseless(p[2], 'L')
					 && i_caseless(p[3], 'O') && i_caseless(p[4], 'B')
					 && i_caseless(p[5], 'A') && i_caseless(p[6], 'L')
					 && p[7] == '?' && p[8] == '?' && brrpath_issep(p[9])) {
						/* Path is '\Global??\*' */
						return i_pfx("\\Global??\\", .type = pfx_global, .abs=1);
					}
				} else if (l >= sizeof("\\??\\") - 1) {
					if (p[1] == '?' && p[2] == '?' && brrpath_issep(p[3])) {
						/* Path is '\??\*' */
						return i_pfx("\\??\\", .type = pfx_global, .abs=1);
					}
				}
				/* Path is something else */
			}
		}
	}
	return (i_pfx_t){0};
}
#endif

/* Returns the length of the canonicalized path without the null terminator
 * max_len includes the null terminator
 * */
static inline brrsz BRRCALL
i_canon(const char *restrict const source, brrsz length,
	char *restrict const dst, brrpath_path_len_t max_len,
	brrpath_inf_t *restrict const flags)
{
	brrpath_inf_t f = {0};
	brrsz i = 0, o = 0;
	#if brrplat_dos
	{ const i_pfx_t pfx = i_get_pfx_type(source, length);
		if (pfx.pfx_type == pfx_none) {
			/* Need not concern ourselves with the prefix-less case. */
		} else if (pfx.pfx_type == pfx_drive || pfx.pfx_type == pfx_global) {
			if (u_cpy_limit(dst, max_len, &o, source, length, &i, pfx.l))
				goto _E;
		} else if (pfx.pfx_type == pfx_unc || pfx.pfx_type == pfx_namespace) {
			/* No processing is needed for unc/namespace paths, they are parsed verbatim */
			u_cpy_limit(dst, max_len, &o, source, length, &i, length);
			dst[o] = 0;
			*flags = f;
			return o;
		}
		f = pfx.f;
	}
	#endif
	/* Dos-specific prefixes taken care of, now to consume the path proper */
	if (brrpath_issep(source[i])) {
		f.abs = 1;
		while (i < length && brrpath_issep(source[i])) ++i;
		if (o < max_len)
			dst[o++] = brrpath_separator;
	}

	static char comp[brrpath_max_name];
	brru1 n_seps = 0; /* Will wrap over and under appropriately; this setup should only fail if there are more than 256 '../' elements in a row */
	#define max_path_seps 256 /* Does this seem like a reasonable maximum on seps (components) in the output? */
	brrpath_path_len_t seps[max_path_seps] = {o};
	while (i < length && o < max_len) {
		int comp_len = 0;
		/* Read only the valid characters of the current component */
		while (i < length && !brrpath_issep(source[i])) {
			if (comp_len < sizeof(comp) && brrpath_norm(source[i]))
				comp[comp_len++] = source[i];
			++i;
		}
		comp[comp_len] = 0;
		/* Skip consecutive separators */
		while (i < length && brrpath_issep(source[i])) ++i;

		if (comp[0] == '.' && comp_len == 1) {
			/* do literally nothing */
			if (o == 0) {
				/* the path starts with a '.' and isn't absolute, therefore this component must actually be
				 * added */
				if (o + 1 > max_len)
					break;
				dst[o++] = '.';
				if (o + 1 > max_len)
					break;
				dst[o++] = brrpath_separator;
				#if brrtools_debug
				dst[o] = 0;
				#endif
				seps[++n_seps] = o;
			}
		} else if (comp[0] == '.' && comp[1] == '.' && comp_len == 2) {
			if (o != seps[0] && seps[n_seps - (brru1)1] >= seps[0]) {
				/* If we have previous valid components to remove, remove them by simply decrementing the
				 * output write position */
				o = seps[--n_seps];
				#if brrtools_debug
				dst[o] = 0;
				#endif

			} else if (!f.abs) {
				/* Otherwise, we've run out of valid components and have to add the current to the beginning
				 * of the output */
				/* But if the path is absolute, '/' and '/..' are the same path, so no need to add any '..' */
				if (o + 2 >= max_len)
					break; /* opt not to write an obviously invalid component */
				dst[o++] = '.'; dst[o++] = '.';
				if (o + 1 >= max_len)
					break;

				dst[o++] = brrpath_separator;
				#if brrtools_debug
				dst[o] = 0;
				#endif

				seps[0] = o;
				n_seps = 0;
			}
		} else {
			/* add the component like normal */
			if (u_cpy_limit(dst, max_len, &o, comp, comp_len, NULL, comp_len))
				break;
			if (o + 1 > max_len)
				break;
			dst[o++] = brrpath_separator;
			#if brrtools_debug
			dst[o] = 0;
			#endif
			seps[++n_seps] = o;
		}
	}
#if brrplat_dos
_E:
#endif
	if (o > 1 && brrpath_issep(dst[o-1])) {
		/* If the last char is a separator, remove it, but only if it's not the only character (i.e. the root
		 * dir) */
		o--;
	}
	dst[o] = 0;
	*flags = f;
	return o;
}

void BRRCALL
brrpath_free(brrpath_t *const path)
{
	if (!path)
		return;
	if (path->full)
		free(path->full);
	memset(path, 0, sizeof(*path));
}

brrpath_t BRRCALL
brrpath_canonicalize(const char *const source, brrsz length)
{
	static char path[brrpath_max_path + 1];
	if (!length)
		return (brrpath_t){0};

	brrpath_t P = {0};
	P.len = i_canon(source, length, path, sizeof(path), (void*)&P.inf);
	if (!(P.full = malloc(P.len + 1))) {
		brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate %zu bytes for canonicalized version of '%s'", P.len + 1, source);
		return (brrpath_t){0};
	}
	memcpy(P.full, path, P.len + 1);
	brrpath_update_lengths(&P);
	return P;
}

#if brrtools_debug
#define _print_bits(_dst_, _v_, _len_, _name_) do {\
	char *_str;\
	switch (sizeof(_v_)) {\
		case 1: _str = "%s:%*s 0x%02x '"; break;\
		case 2: _str = "%s:%*s 0x%04x '"; break;\
		case 4: _str = "%s:%*s 0x%08x '"; break;\
		case 8: _str = "%s:%*s 0x%016x '"; break;\
	}\
	printf(_str, _name_, (_len_) > sizeof(_name_) ? ((_len_) - sizeof(_name_)) : 0, "", _v_);\
	for (int i = 0; i < 8 * sizeof(_v_); ++i) {\
		fputc(((_v_) >> (8 * sizeof(_v_) - i - 1)) & 0x01 ? '1' : '0', _dst_);\
	}\
	printf("'\n");\
} while (0)
#else
#define _print_bits(_dst_, _v_, _len_, _name_)
#endif

#define _s_flag(_s_, _flag_) (((_s_) & (_flag_)) == (_flag_))
static BRR_inline void BRRCALL
i_inf_from_st(brrpath_inf_t *const inf, brrpath_stat_t st)
{
	brrpath_inf_t i = *inf;
#if brrplat_unix
	if (_s_flag(st.st_mode, S_IFREG))
		i.type = brrpath_type_file;
	else if (_s_flag(st.st_mode, S_IFDIR))
		i.type = brrpath_type_dir;
	else if (_s_flag(st.st_mode, S_IFBLK) || _s_flag(st.st_mode, S_IFCHR))
		i.type = brrpath_type_device;
	else if (_s_flag(st.st_mode, S_IFIFO) || _s_flag(st.st_mode, S_IFSOCK))
		i.type = brrpath_type_other;
	else
		i.type = brrpath_type_invalid;
	/* TODO can any flags be gotten from st_mode? */
	brrpath_set_size(i.size, st.st_size);
#elif brrplat_dos
	DWORD at = st.dwFileAttributes;
	/* This logic seems to work in windows, however it fails in wine (specifically, it seems 'dwReserved0'
	 * aka the reparse tag does not get set properly) */
	//_print_bits(stdout, at, sizeof("reserved 0"), "at");
	if (at & FILE_ATTRIBUTE_DEVICE)
		i.type = brrpath_type_device;
	else if (at & FILE_ATTRIBUTE_DIRECTORY)
		i.type = brrpath_type_dir;
	else if ((at & FILE_ATTRIBUTE_READONLY) || brr_bits(at, FILE_ATTRIBUTE_NORMAL) ||
		!brr_bit(at, FILE_ATTRIBUTE_VIRTUAL))
		i.type = brrpath_type_file;
	else
		i.type = brrpath_type_other;

	if (at & FILE_ATTRIBUTE_REPARSE_POINT) {
		//_print_bits(stdout, st.dwReserved0, sizeof("reserved 0"), "reserved 0");
		//_print_bits(stdout, st.dwReserved1, sizeof("reserved 1"), "reserved 1");
		if (st.dwReserved0 == IO_REPARSE_TAG_SYMLINK)
			brr_bit_mask_set(i.flags, brrpath_flag_reparse, brrpath_flag_symlink);
		else if (st.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT)
			brr_bit_mask_set(i.flags, brrpath_flag_reparse, brrpath_flag_mount);
		else
			i.flags |= brrpath_flag_reparse;
	}
	if (at & FILE_ATTRIBUTE_ARCHIVE) i.flags |= FILE_ATTRIBUTE_ARCHIVE;
	if (at & FILE_ATTRIBUTE_COMPRESSED) i.flags |= brrpath_flag_compressed;
	if (at & FILE_ATTRIBUTE_ENCRYPTED) i.flags |= brrpath_flag_encrypted;
	brrpath_set_size(i.size, _win_ulrg(st.nFileSizeHigh, st.nFileSizeLow));
#endif
	*inf = i;
}

int BRRCALL
brrpath_stat(brrpath_t *const path)
{
	if (!path || !path->full) {
		brrapi_error(BRRAPI_E_ARGERR, "brrpath_stat given NULL path");
		return -1;
	}
	brrpath_inf_t i = path->inf;
#if brrplat_unix
{	struct stat st = {0};
	if (lstat(path->full, &st)) {
		if (errno != ENOENT) {
			brrapi_error(BRRAPI_E_OSERR, "Failed to stat path '%s'", path->full);
			return -1;
		}
		i.exists = 0;
	} else {
		i.exists = 1;
		if (_s_flag(st.st_mode, S_IFLNK)) {
			i.flags |= brrpath_flag_symlink;

			/* Stat the target to determine the filetype; question is, what if we stat a symlink to a
			 * symlink? Does 'stat' go all the way down, or just to the next?
			 * For now, I'll just assume 'stat' stats the actual file that exists, unless it's a broken
			 * symlink
			 * */
			if (stat(path->full, &st)) {
				if (errno != ENOENT) {
					brrapi_error(BRRAPI_E_OSERR, "Failed to stat symbolic link '%s'", path->full);
					return -1;
				}
				i.flags |= brrpath_flag_broken;
			}
		}
		i_inf_from_st(&i, st);
	}
}
#elif brrplat_dos
{	WIN32_FIND_DATA st = {0}; HANDLE h;
	i.exists = 1;
	if (INVALID_HANDLE_VALUE == (h = FindFirstFile(path->full, &st))) {
		int e = GetLastError();
		if (e != ERROR_FILE_NOT_FOUND) {
			brrapi_error(BRRAPI_E_OSERR, "Failed to stat path '%s'", path->full);
			return -1;
		}

		if (!GetFileAttributesEx(path->full, GetFileExInfoStandard, &st)) {
			e = GetLastError();
			if (e != ERROR_FILE_NOT_FOUND && e != ERROR_PATH_NOT_FOUND) {
				FindClose(h);
				brrapi_error(BRRAPI_E_OSERR, "Failed to stat symbolic link '%s'", path->full);
				return -1;
			}
			i.exists = 0;
		}
		i.flags |= brrpath_flag_broken;
	}
	i_inf_from_st(&i, st);
	FindClose(h);
}
#endif
	path->inf = i;
	return 0;
}

const brrpath_t *BRRCALL
brrpath_cwd(void)
{
	static char dir[brrpath_max_path + 1];
	static brrpath_t path = {
		.full = dir,
	};
#if brrplat_unix
	if (!getcwd(dir, sizeof(dir))) {
		brrapi_error(BRRAPI_E_OSERR, "Couldn't get the current working directory");
		return NULL;
	}
	path.len = strlen(dir);
#elif brrplat_dos
	path.len = GetCurrentDirectory(sizeof(dir), dir);
	if (!path.len) {
		brrapi_error(BRRAPI_E_OSERR, "Couldn't get the current working directory");
		return NULL;
	}
	if (path.len >= sizeof(dir) - 1) {
		path.len = sizeof(dir) - 1;
		dir[path.len] = 0;
	}
#endif
	if (brrpath_stat(&path))
		return NULL;
	return &path;
}

/* Whether or not an error occurred during walk */
static int i_walk_code = 0;
static brrpath_walk_settings_t i_walk;
#if brrplat_unix
static int i_nftw_walk(const char *restrict const path, const struct stat *restrict st, int type, struct FTW *restrict F)
{
	if ((unsigned)F->level < i_walk.start_depth)
		return FTW_CONTINUE;
	if ((unsigned)F->level > i_walk.max_depth)
		return FTW_SKIP_SIBLINGS;
	if (type == FTW_NS) /* Failed to stat path */
		return FTW_CONTINUE;

	/* You know, it might just be ideal to give the user what they need to produce the brrpath themselves,
	 * rather than allocating a path, asking the user if they want it, only to then free it when they don't;
	 * 'better to return early' and all that, but for right now, I want something that works, efficiency be
	 * damned. */
	brrpath_t P = brrpath_canonicalize(path, strlen(path));
	/* I know it's stupid to stat again, when nftw does it for me already, but doing it again allows me to
	 * check for symlinks */
	if (!P.full || brrpath_stat(&P)) {
		i_walk_code = 1;
		brrpath_free(&P);
		return FTW_STOP;
	}
	brrpath_walk_code_t code = i_walk.walker(&P, F->level, i_walk.user);
	if (code & brrpath_walk_flag_stop) {
		if (!brr_bit(code, brrpath_walk_flag_consumed))
			brrpath_free(&P);
		if (brr_bit(code, brrpath_walk_flag_error))
			i_walk_code = 1;
		return FTW_STOP;
	}
	if (!brr_bit(code, brrpath_walk_flag_consumed))
		brrpath_free(&P);
	switch (code & brrpath_walk_code_mask) {
		case brrpath_walk_code_skip_siblings: return FTW_SKIP_SIBLINGS;
		case brrpath_walk_code_skip_tree: return FTW_SKIP_SUBTREE;
		default: return FTW_CONTINUE;
	}
	return FTW_CONTINUE;
}
#elif brrplat_dos
#define walk_glob "\\*.*"
static int i_dos_walk(char *restrict const path, brrpath_path_len_t len, WIN32_FIND_DATA *restrict const find_data, brrsz current_depth)
{
	/* TODO This might be possible to do without recursion, will have to look into it when I'm curious */
	HANDLE start;
	{
		for (int i = 0; i < sizeof(walk_glob); ++i)
			path[len + i] = walk_glob[i];
		if (INVALID_HANDLE_VALUE == (start = FindFirstFile(path, find_data))) {
			return -1;
		}
	}
	path[len] = 0;

	do {
		if (!strcmp(find_data->cFileName, "..") || !strcmp(find_data->cFileName, ".")) {
			continue;
		}

		if (len + 1 < brrpath_max_path) {
			int name_len = strlen(find_data->cFileName);
			path[len++] = brrpath_separator;
			if (len + name_len < brrpath_max_path) {
				memcpy(path + len, find_data->cFileName, name_len);
				len += name_len;
			} else {
				memcpy(path + len, find_data->cFileName, brrpath_max_path - len);
				len = brrpath_max_path;
			}
		}

		brrpath_t P = brrpath_canonicalize(path, len);
		brrpath_walk_code_t code = 0;
		if (!P.full || brrpath_stat(&P)) {
			i_walk_code = 1;
			brrpath_free(&P);
			break;
		}
		brrpath_path_len_t dir_len = P.dir_len;
		brrpath_type_t type = P.type;
		brrpath_flag_t flags = P.flags;
		memcpy(path, P.full, P.len);
		len = P.len;
		if (current_depth < i_walk.start_depth) {
			brrpath_free(&P);
		} else {
			code = i_walk.walker(&P, current_depth, i_walk.user);
			if (!brr_bit(code, brrpath_walk_flag_consumed))
				brrpath_free(&P);
			if (code & brrpath_walk_flag_stop) {
				if (code & brrpath_walk_flag_error)
					i_walk_code = 1;
				break;
			}
			if (code & brrpath_walk_code_skip_siblings) {
				break;
			}
		}
		if (type == brrpath_type_dir &&
			(!(flags & brrpath_flag_reparse) || (flags & brrpath_flag_broken)) &&
			!(code & brrpath_walk_code_skip_tree) &&
			current_depth < i_walk.max_depth) {
			i_dos_walk(path, len, find_data, current_depth + 1);
		}
		len = dir_len - 1;
		path[len] = 0;
	} while (!i_walk_code && FindNextFile(start, find_data));
	CloseHandle(start);
	/* TODO check for FindNextFile error */
	return i_walk_code;
}
#endif

int BRRCALL
brrpath_walk(const brrpath_t *const start, brrpath_walk_settings_t settings)
{
#if brrplat_dos
	static char path[brrpath_max_path + sizeof(walk_glob)] = {0};
#else
	static char path[brrpath_max_path + 1] = {0};
#endif
	static brrsz len = 0;
	if (!start || !start->full) {
		brrapi_error(BRRAPI_E_ARGERR, "brrpath_walk given NULL start point");
		return -1;
	} else if (!start->exists) {
		brrapi_error(BRRAPI_E_ARGERR, "brrpath_walk given start point which doesn't exist '%s'", start->full);
		return -1;
	} else if (!settings.walker) {
		brrapi_error(BRRAPI_E_ARGERR, "brrpath_walk given NULL walker");
	} else if (settings.max_depth <= settings.start_depth) {
		return 0;
	} else if (start->type != brrpath_type_dir) {
		len = start->dir_len;
	} else {
		len = start->len;
	}
	memcpy(path, start->full, len);
	path[len] = 0;
	i_walk = settings;
	i_walk_code = 0;
#if brrplat_unix
	/* I know that requiring _GNU_SOURCE for FTW_SKIP_SIBLINGS/_SKIP_SUBTREE goes against the whole
	 * 'portability' thing, but gnu nftw is a far better implementation than what I could come up with on my
	 * own, avoiding race conditions and whatever other benefits and considerations the official
	 * implementation takes.
	 * Also it's not thread-safe (I think?); that'd be nice to fix.
	 * Maybe one day I can do a custom solution, but that day is not today. */
	return nftw(path, i_nftw_walk, 15, FTW_PHYS | FTW_MOUNT | FTW_ACTIONRETVAL) < 0 ? -1 : 0;
#elif brrplat_dos
	WIN32_FIND_DATA fd;
	brrpath_t pp = brrpath_canonicalize(start->full, start->len);
	if (!pp.full || brrpath_stat(&pp)) {
		i_walk_code = 1;
		brrpath_free(&pp);
		return -1;
	}
	brrpath_walk_code_t code = i_walk.walker(&pp, 0, i_walk.user);
	if (!(code & brrpath_walk_flag_consumed))
		brrpath_free(&pp);
	if (code & brrpath_walk_flag_error) {
		i_walk_code = 1;
		return -1;
	}
	switch (code & brrpath_walk_code_mask) {
		case brrpath_walk_code_skip_tree: return 0;
		case brrpath_walk_code_skip_siblings:;
	}
	return -i_dos_walk(path, len, &fd, 1);
#endif
}
