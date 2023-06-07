#include "brrstat.h"

#include <errno.h>

#include "brrtools/brrapi.h"
#include "brrtools/brrplat.h"

#if brrplat_unix
# include <sys/stat.h>
# include <unistd.h>
typedef struct stat stat_t;
#elif brrplat_dos
# include <aclapi.h>
# define _win_ulrg(_hi_, _lo_) (((ULARGE_INTEGER){.u={.LowPart=(_lo_),.HighPart=(_hi_)}}).QuadPart)
typedef WIN32_FIND_DATA stat_t;
#endif

static BRR_inline void BRRCALL
i_inf_from_st(brrstat_t *const inf, stat_t st)
{
	brrstat_t i = *inf;
#if brrplat_unix
#elif brrplat_dos
#endif
	*inf = i;
}

int
brrstat(brrstat_t *restrict const Stat, const char *const path)
{
	if (!path) {
		brrapi_error(BRRAPI_E_ARGERR, "brrstat given NULL path");
		return -1;
	}
	brrstat_t res = {0};
	#if brrplat_unix
	{	stat_t st = {0};
		if (lstat(path, &st)) {
			if (errno != ENOENT) {
				brrapi_error(BRRAPI_E_OSERR, "Failed to stat path '%s'", path);
				return -1;
			}
		} else {
			if (!(st.st_mode & S_IFLNK)) {
				res.ltype = brrstat_ltype_hardlink;
			} else {
				res.ltype = brrstat_ltype_softlink;
				/* Stat the target to determine the filetype; question is, what if we stat a symlink to a symlink? Does 'stat' go all the way down, or just to the next?
				 * For now, I'll just assume 'stat' stats the actual file that exists, unless it's a broken symlink */
				if (stat(path, &st)) {
					if (errno != ENOENT) {
						brrapi_error(BRRAPI_E_OSERR, "Failed to stat symbolic link '%s'", path);
						return -1;
					}
					res.ltype = brrstat_ltype_broken;
				}
			}

			if (res.ltype != brrstat_ltype_none) {
				res.size = st.st_size;
				if (st.st_mode & S_IFREG)
					res.ftype = brrstat_ftype_file;
				else if (st.st_mode & S_IFDIR)
					res.ftype = brrstat_ftype_directory;
				else
					res.ftype = st.st_mode; /* Don't know, the user can do their own thing if inclined */
			}
		}
	}
	#elif brrplat_dos
	{	WIN32_FIND_DATA st = {0};
		HANDLE h;
		/* TODO I think this statting logic is wrong
		 * I think it's very wrong actually.
		 * Specifically, what does FindFirstFile do when given a directory?
		 * I think to fix, I have to use GetFileAttributesEx instead.
		 * */
		res.ltype = brrstat_ltype_hardlink;
		if (INVALID_HANDLE_VALUE == (h = FindFirstFile(path, &st))) {
			int e = GetLastError();
			if (e != ERROR_FILE_NOT_FOUND) {
				brrapi_error(BRRAPI_E_OSERR, "Failed to stat path '%s'", path);
				return -1;
			}

			if (!GetFileAttributesEx(path, GetFileExInfoStandard, &st)) {
				e = GetLastError();
				if (e != ERROR_FILE_NOT_FOUND && e != ERROR_PATH_NOT_FOUND) {
					FindClose(h);
					brrapi_error(BRRAPI_E_OSERR, "Failed to stat symbolic link '%s'", path);
					return -1;
				}
				res.ltype = brrstat_ltype_none;
			}
			res.ltype = brrstat_ltype_broken;
		}

		if (res.ltype != brrstat_ltype_hardlink) {
			DWORD at = st.dwFileAttributes;
			/* This logic seems to work in windows, however it fails in wine (specifically, it seems 'dwReserved0'
			 * aka the reparse tag does not get set properly) */
			//_print_bits(stdout, at, sizeof("reserved 0"), "at");
			if (at & FILE_ATTRIBUTE_DEVICE)
				res.ftype = st.dwFileAttributes;
			else if (at & FILE_ATTRIBUTE_DIRECTORY)
				res.ftype = brrstat_ftype_directory;
			else if ((at & FILE_ATTRIBUTE_READONLY) || (at & FILE_ATTRIBUTE_NORMAL) || !(at & FILE_ATTRIBUTE_VIRTUAL))
				res.ftype = brrstat_ftype_file;
			else
				res.ftype = st.dwFileAttributes;

			if (at & FILE_ATTRIBUTE_REPARSE_POINT) {
				//_print_bits(stdout, st.dwReserved0, sizeof("reserved 0"), "reserved 0");
				//_print_bits(stdout, st.dwReserved1, sizeof("reserved 1"), "reserved 1");
				if (st.dwReserved0 == IO_REPARSE_TAG_SYMLINK)
					res.ltype = brrstat_ltype_softlink;
				else if (st.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT)
					res.ltype = brrstat_ltype_reparse;
			}
			res.size =_win_ulrg(st.nFileSizeHigh, st.nFileSizeLow);
				FindClose(h);
		}
	}
	#endif
	*Stat = res;
	return 0;
}
