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

#include "brrtools/brrapi.h"
#include "brrtools/noinstall/utils.h"
#include "brrtools/brrplatform.h"
#include "brrtools/brrlib.h"

#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

//#if defined(BRRPLATFORMTYPE_UNIX)
//# include <ftw.h> // nftw()
//#endif

#if defined(BRRPLATFORMTYPE_WINDOWS)
# if BRRTOOLS_TARGET_BIT == 64
#  define statpath _stat64
#  define fstatpath _fstat64
typedef struct __stat64 pathstT;
# else
#  define statpath _stat32
#  define fstatpath _fstat32
#  if defined(BRRPLATFORMTYPE_MINGW)
typedef struct _stat32 pathstT;
#  else
typedef struct __stat32 pathstT;
#  endif // BRRPLATFORMTYPE_MINGW
# endif // BRRTOOLS_TARGET_BIT
# define lstatpath statpath

# define TYPEDIR(md) (HASFLAG((md), _S_IFDIR))
# define TYPEFIL(md) (HASFLAG((md), _S_IFREG))
# define TYPELNK(md) (false)
#else
# define statpath stat
# define fstatpath fstat
# define lstatpath lstat
typedef struct stat pathstT;

# define TYPEDIR(md) (S_ISDIR(md))
# define TYPEFIL(md) (S_ISREG(md))
# define TYPELNK(md) (S_ISLNK(md))
#endif

#if defined(BRRPLATFORMTYPE_WINDOWS)
# define PATHSEPCHR '\\'
# define PATHSEPSTR "\\"
# define ROOTSTR "C:"
#else
# define PATHSEPCHR '/'
# define PATHSEPSTR "/"
# define ROOTSTR "/"
#endif
#define MAXNAME 255

#define CURDIR "."
#define PARDIR ".."

#if defined(BRRPLATFORMTYPE_WINDOWS)
const brrb1 csepaths = true;
#else
const brrb1 csepaths = false;
#endif

brrb1 BRRCALL
brrpath_absolute(const char *const path)
{
	brrb1 r = false;
	if (path && path[0] != 0) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		brrsz l = strlen(path);
		if (path[0] == PATHSEPCHR ||
		 (l >= 2 && path[0] == path[1] && path[1] == PATHSEPCHR) ||
		 (l >= 3 && isalpha(path[0]) && path[1] == ':' && path[2] == PATHSEPCHR))
#elif defined(BRRPLATFORMTYPE_UNIX)
		if (path[0] == PATHSEPCHR)
#endif
			r = true;
	}
	return r;
}

static brrb1 isnamechar(char ch) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
	// todo I don't think this is exhaustive
	return !(ch==PATHSEPCHR||
			ch=='<'||ch=='>'||
			ch==':'||ch=='"'||
			ch=='/'||ch=='|'||
			ch=='?'||ch=='*'||
			ch<32);
#elif defined(BRRPLATFORMTYPE_UNIX)
	// Just generally disallow backslashes for now.
	return !(ch == PATHSEPCHR||ch=='\\');
#else
	return !(ch == PATHSEPCHR||ch=='\\');
#endif
}

static char *cleanflatpath(const char *const path) {
	char *res = NULL;
	if (path && path[0] != 0) {
		brrsz l = strlen(path);
		brrsz c = 0;
		if (!brrlib_alloc((void **)&res, l + 1, 1))
			return NULL;
		for (brrsz i = 0; i < l; ++i) {
			if (isnamechar(path[i]) ||
			   (i + 1 == l && path[i] == PATHSEPCHR)) {
				res[c++] = path[i];
			} else if (i + 1 < l && isnamechar(path[i+1])) {
				res[c++] = path[i];
			}
		}
		if (!brrlib_alloc((void **)&res, c+1, 0)) {
			brrlib_alloc((void **)&res, 0, 0);
			return NULL;
		}
		res[c] = 0;
	}
	return res;
}
brrpath_statT BRRCALL
brrpath_stat(const char *const path, brrb1 followlink)
{
	brrpath_statT st = {0};
	if (path && path[0] != 0) {
		pathstT s = {0};
		int err = 0;
		if (followlink)
			err = statpath(path, &s);
		else
			err = lstatpath(path, &s);
		if (err != 0) {
			if (errno != ENOENT) {
#if defined(GFS_PLATFORMTYPE_UNIX)
				if (errno == ENOTDIR) st.stat_error |= brrpath_statinvalidtype;
				else
#endif
				st.stat_error |= brrpath_staterr;
			} else {
				st.type |= brrpath_directory;
			}
		} else {
			st.size = s.st_size;
			if (TYPEDIR(s.st_mode)) {
				st.type |= brrpath_directory;
			} else if (TYPEFIL(s.st_mode)) {
				st.type |= brrpath_file;
			} else if (TYPELNK(s.st_mode)) {
				st.type |= brrpath_link;
			} else {
				st.type |= brrpath_irregular;
			}
			st.exists = true;
		}
	} else {
		st.stat_error |= brrpath_statinvalidpath;
	}
	if (!st.stat_error)
		st.absolute = brrpath_absolute(path);
	return st;
}
brrpath_statT BRRCALL
brrpath_fstat(int fd)
{
	brrpath_statT st = {0};
	pathstT s = {0};
	if (fstatpath(fd, &s) != 0) {
		st.stat_error |= brrpath_staterr;
	} else {
		st.size = s.st_size;
		if (TYPEDIR(s.st_mode)) {
			st.type |= brrpath_directory;
		} else if (TYPEFIL(s.st_mode)) {
			st.type |= brrpath_file;
		} else if (TYPELNK(s.st_mode)) {
			st.type |= brrpath_link;
		} else {
			st.type |= brrpath_irregular;
		}
		st.exists = true;
	}
	return st;
}

brrb1 BRRCALL
brrpath_exists(const char *const path, brrb1 followlink)
{
	brrpath_statT s = brrpath_stat(path, followlink);
	return s.stat_error == 0 && s.exists;
}

brrpath_typeT BRRCALL
brrpath_type(const char *const path, brrb1 followlink)
{
	return brrpath_stat(path, followlink).type;
}

/*
brrb1 BRRCALL
brrpath_makepath(const char *const path, brrb1 parents, brrb1 clean)
{
	brrb1 err = false;
	brrpath_statT st = brrpath_stat(path, true);
	if (st.stat_error)
		return false;
	// No need to create the current directory.
	if (!st.exists && path->elementcount > 0) {
		// No need to create the root directory.
		if (!ISABSP(path) || (ISABSP(path) && path->elementcount > 1)) {
			stgT shortpath = stgnull;
			sz ofs = 0;
			flatpath(&shortpath, path->elements, path->elementcount, ISABSP(path));

			if (ISABSP(path)) {
#if defined(GFS_PLATFORMTYPE_WINDOWS)
				ofs = 3; // todo Not always correct.
#else
				ofs = 1;
#endif
			}
			for (;err == ERR_PATH_NONE; ++ofs) {
				b1 e = false;
				char stor;
				// Skip to next path separator.  while (ofs < shortpath.length && shortpath.cstr[ofs] != pathsep) ++ofs;
				if (ofs == shortpath.length) break;

				stor = shortpath.cstr[ofs];
				shortpath.cstr[ofs] = 0;
				// Try to make subdirectory
#if defined(GFS_PLATFORMTYPE_WINDOWS)
				e = (0 == _mkdir(shortpath.cstr));
#else
				// todo Create mode.
				e = (0 == mkdir(shortpath.cstr, 0755));
#endif
				if (e) {
					if (errno != EEXIST) {
						LOG_ERROR("Failed creating directory tree to '%s' at '%s' : %s",
						        path->fullpath.cstr, shortpath.cstr, ERRSTR);
						err |= ERR_PATH_CREATE;
					}
				}
				shortpath.cstr[ofs] = stor;
			}
		}
		if (!err) {
			path->meta |= PATH_NEW | PATH_EXISTS;
		}
	}
	return err;
}
*/
