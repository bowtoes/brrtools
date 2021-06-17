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
#include "brrtools/brrstr.h"
#include "brrtools/noinstall/utils.h"
#include "brrtools/brrplatform.h"
#include "brrtools/brrlib.h"

#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

#if defined(BRRPLATFORMTYPE_WINDOWS)
# include <windows.h>
#else
# include <unistd.h>
#endif

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
# define BRRPATH_MAX 260
#else
# define PATHSEPCHR '/'
# define PATHSEPSTR "/"
# define ROOTSTR "/"
# define BRRPATH_MAX 4097
#endif
#define BRRPATH_MAXNAME 255

#define CURDIR "."
#define PARDIR ".."

#if defined(BRRPLATFORMTYPE_WINDOWS)
static const brrb1 casepaths = false;
#else
static const brrb1 casepaths = true;
#endif

static brrb1 BRRCALL
isnamechar(char ch) {
#if defined(BRRPLATFORMTYPE_WINDOWS) // todo I don't think this is exhaustive
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
	return !(ch == PATHSEPCHR||ch=='\\'||ch=='/');
#endif
}
static brrstrT BRRCALL
cleanflatpath(const char *const path) {
	brrstrT str = brrstr_new(path, -1);
	brrsz c = 0, l = brrstr_strlen(&str);
	for (brrsz i = 0; i < l; ++i) {
		if (isnamechar(path[i]) ||
		   (i + 1 == l && path[i] == PATHSEPCHR) ||
		   (i + 1 < l && isnamechar(path[i+1]))) {
			((char *)brrstr_cstr(&str))[c++] = path[i];
		}
	}
	brrstr_resize(&str, c + 1);
	return str;
}
static brrpath_typeT BRRCALL
gettype(pathstT s) {
	brrpath_typeT t = 0;
	if (TYPEDIR(s.st_mode)) {
		t |= brrpath_type_directory;
	} else if (TYPEFIL(s.st_mode)) {
		t |= brrpath_type_file;
	} else if (TYPELNK(s.st_mode)) {
		t |= brrpath_type_link;
	} else {
		t |= brrpath_type_irregular;
	}
	return t;
}
static brrpath_permsT BRRCALL
getperms(pathstT stat) {
	brrpath_permsT perms = brrpath_perms_none;
#if defined(BRRPLATFORMTYPE_WINDOWS)
	if (stat.st_mode & _S_IREAD)
		perms |= brrpath_perms_readable;
	if (stat.st_mode & _S_IWRITE)
		perms |= brrpath_perms_writable;
	if (stat.st_mode & _S_IEXEC)
		perms |= brrpath_perms_executable;
#else
	/* Unix permissions are complicated; this doesn't take into account acls
	 * or anything extra. Probably not safe. */
	if (geteuid() == stat.st_uid) {
		if (stat.st_mode & S_IRUSR)
			perms |= brrpath_perms_readable;
		if (stat.st_mode & S_IWUSR)
			perms |= brrpath_perms_writable;
		if (stat.st_mode & S_IXUSR)
			perms |= brrpath_perms_executable;
	} else if (getgid() == stat.st_gid) {
		if (stat.st_mode & S_IRGRP)
			perms |= brrpath_perms_readable;
		if (stat.st_mode & S_IWGRP)
			perms |= brrpath_perms_writable;
		if (stat.st_mode & S_IXGRP)
			perms |= brrpath_perms_executable;
	} else {
		if (stat.st_mode & S_IROTH)
			perms |= brrpath_perms_readable;
		if (stat.st_mode & S_IWOTH)
			perms |= brrpath_perms_writable;
		if (stat.st_mode & S_IXOTH)
			perms |= brrpath_perms_executable;
	}
#endif
	return perms;
}

brrstrT BRRCALL
brrpath_cwd(void) {
	static char tmp[BRRPATH_MAX] = {0};
#if defined(BRRPLATFORMTYPE_WINDOWS)
	GetCurrentDirectory(BRRPATH_MAX, tmp);
#else
	getcwd(tmp, BRRPATH_MAX);
#endif
	return brrstr_new(tmp, BRRPATH_MAX - 1);
}

char *BRRCALL
brrpath_flatten(const char *const path)
{
	if (!path)
		return NULL;
}

brrb1 BRRCALL
brrpath_is_absolute(const char *const path)
{
	if (path && path[0] != 0) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		brrsz l = strlen(path);
		// This is likely incomplete
		if (path[0] == PATHSEPCHR ||
		 (l >= 2 && path[0] == PATHSEPCHR && path[1] == PATHSEPCHR) ||
		 (l >= 3 && isalpha(path[0]) && path[1] == ':' && path[2] == PATHSEPCHR))
#elif defined(BRRPLATFORMTYPE_UNIX)
		if (path[0] == PATHSEPCHR)
#endif
			return true;
	}
	return false;
}

brrpath_statT BRRCALL
brrpath_stat(const char *const path, int follow_link)
{
	brrpath_statT st = {0};
	pathstT s = {0};
	if (!path || path[0] == 0) {
		st.error |= brrpath_stat_invalid_path;
		return st;
	}
	if (0 == (follow_link?statpath(path, &s):lstatpath(path, &s))) {
		st.size = s.st_size;
		st.exists = true;
		st.type = gettype(s);
		st.perms = getperms(s);
		st.is_absolute = brrpath_is_absolute(path);
	} else {
		if (errno != ENOENT) {
#if defined(BRRPLATFORMTYPE_UNIX)
			if (errno == ENOTDIR)
				st.error |= brrpath_stat_invalid_type;
			else
#endif
			st.error |= brrpath_stat_error;
		} else {
			st.type |= brrpath_type_directory;
			st.is_absolute = brrpath_is_absolute(path);
		}
	}
	return st;
}

brrb1 BRRCALL
brrpath_exists(const char *const path, int follow_link)
{
	/* TODO is there a version of `access` that doesn't follow symlinks? */
	brrpath_statT s = brrpath_stat(path, follow_link);
	return s.error == 0 && s.exists;
}

brrpath_typeT BRRCALL
brrpath_type(const char *const path, int follow_link)
{
	return brrpath_stat(path, follow_link).type;
}

/*
#if defined(BRRPLATFORMTYPE_UNIX)
# include <ftw.h> // nftw()
#endif

brrb1 BRRCALL
brrpath_makepath(const char *const path, brrb1 parents, brrb1 clean)
{
	brrb1 err = false;
	brrpath_statT st = brrpath_stat(path, true);
	if (st.error)
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
