#include "brrtools/brrpath.h"

#include <errno.h>
#if brrplat_unix
# include <sys/stat.h>
#elif brrplat_dos
# include <aclapi.h>
# define _win_ulrg(_hi_, _lo_) (((ULARGE_INTEGER){.u={.LowPart=(_lo_),.HighPart=(_hi_)}}).QuadPart)
#endif

static BRR_inline brrpath_type_t
i_type_from_attr(brru8 attr)
{
#if brrplat_unix
	if (S_ISLNK(attr))  return brrpath_type_symlink;
	if (S_ISBLK(attr))  return brrpath_type_blockdev;
	if (S_ISCHR(attr))  return brrpath_type_chardev;
	if (S_ISFIFO(attr)) return brrpath_type_pipe;
	if (S_ISREG(attr))  return brrpath_type_file;
	if (S_ISDIR(attr))  return brrpath_type_dir;
	if (S_ISSOCK(attr)) return brrpath_type_socket;
#elif brrplat_dos
	if (attr & FILE_ATTRIBUTE_DEVICE) return brrpath_type_device;
	if (attr & FILE_ATTRIBUTE_DIRECTORY) return brrpath_type_file;
	if (attr & FILE_ATTRIBUTE_REPARSE_POINT) return brrpath_type_reparse;
	if (attr & (FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_SPARSE_FILE)) return brrpath_type_file;
#endif
	return 0;
}
static BRR_inline brrpath_flags_t
i_flags_from_attr(brru8 attr)
{
	brrpath_flags_t f = 0;
#if brrplat_unix
	/* TODO can any flags be gotten from st_mode? */
#elif brrplat_dos
	if (attr & FILE_ATTRIBUTE_SPARSE_FILE) f |= brrpath_flag_sparse;
	if (attr & FILE_ATTRIBUTE_COMPRESSED) f |= brrpath_flag_compressed;
	if (attr & FILE_ATTRIBUTE_ENCRYPTED) f |= brrpath_flag_encrypted;
#endif
	return f;
}
static BRR_inline brrpath_perms_t
i_perms_from_attr(brru8 attr)
{
	brrpath_perms_t p = 0;
	return 0;
#if 0
#if brrplat_unix
	if (attr & S_IRUSR) p |= brrpath_perm_ur;
	if (attr & S_IRGRP) p |= brrpath_perm_gr;
	if (attr & S_IROTH) p |= brrpath_perm_or;

	if (attr & S_IWUSR) p |= brrpath_perm_uw;
	if (attr & S_IWGRP) p |= brrpath_perm_gw;
	if (attr & S_IWOTH) p |= brrpath_perm_ow;

	if (attr & S_IXUSR) p |= brrpath_perm_ux;
	if (attr & S_IXGRP) p |= brrpath_perm_gx;
	if (attr & S_IXOTH) p |= brrpath_perm_ox;
#elif brrplat_dos
	const char *path = (const char *)attr;
	PACL pacl;
	DWORD e = GetNamedSecurityInfo(path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pacl, NULL, NULL);
	if (e != ERROR_SUCCESS) {
		brrapi_sete(BRRAPI_E_OSERR);
		return -1;
	}
#endif
	return p;
#endif
}

brrpath_info_t BRRCALL
brrpath_stat(const char *const path)
{
	brrpath_info_t i = {0};
	if (!path) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return (brrpath_info_t){0};
	}
#if brrplat_unix
	struct stat s = {0};
	if (lstat(path, &s)) {
		if (errno != ENOENT) {
			/* Other error */
			brrapi_sete(BRRAPI_E_OSERR);
			return (brrpath_info_t){.size=BRRSZ_MAX};
		}
		/* Path doesn't exist */
		return (brrpath_info_t){0};
	}
	/* Path exists */
	i.exists = 1;
	i.size = s.st_size;
	i.type = i_type_from_attr(s.st_mode);
	i.flags = i_flags_from_attr(s.st_mode); /* flags from st_mode? */
	if (i.type == brrpath_type_symlink) {
		/* Path is symlink, query info for the target */
		struct stat st = {0};
		if (!stat(path, &st)) {
			/* Target exists */
			i.size = st.st_size;
			i.type |= i_type_from_attr(st.st_mode) << 4;
			i.flags = i_flags_from_attr(st.st_mode); /* flags from st_mode? */
		} else if (errno == ENOENT) {
			/* Target doesn't exist */
			i.flags |= brrpath_flag_broken;
		} else {
			/* Other error */
			brrapi_sete(BRRAPI_E_OSERR);
			return (brrpath_info_t){.size=BRRSZ_MAX};
		}
	}
	/* Success! */
#elif brrplat_dos
	WIN32_FIND_DATA d = {0};
	HANDLE h;
	if (INVALID_HANDLE_VALUE == (h = FindFirstFile(path, &d))) {
		int e = GetLastError();
		if (e != ERROR_FILE_NOT_FOUND) {
			brrapi_sete(BRRAPI_E_OSERR);
			return (brrpath_info_t){.size=BRRSZ_MAX};
		} else {
			/* File failed to be opened, but it might be a broken symlink; gotta check
			 * According to MS docs, if 'FindFirstFile' fails, then 'd' has indeterminate fields; MS docs also
			 * state that 'FindFirstFile' returns info about the symlink in 'd' if it is a symlink, but the
			 * docs fail to outline what happens in the even of an invalid/broken symlink; for this, I must
			 * check manually a second time using 'GetFileAttributesEx'.
			 * */
			WIN32_FILE_ATTRIBUTE_DATA attr = {0};
			if (GetFileAttributesEx(path, GetFileExInfoStandard, &attr)) {
				/* It is a broken symlink */
				i.exists = 1;
				i.size = _win_ulrg(attr.nFileSizeHigh, attr.nFileSizeLow);
				i.type = i_type_from_attr(attr.dwFileAttributes); /* Should be reparse */
				i.flags = brrpath_flag_broken;
				return i;
			} else {
				e = GetLastError();
				if (e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND) {
					/* Path doesn't exist at all, so no error */
					return (brrpath_info_t){0};
				} else {
					/* Failed to stat path in any way */
					brrapi_sete(BRRAPI_E_OSERR);
					return (brrpath_info_t){.size=BRRSZ_MAX};
				}
			}
		}
	}
	/* Otherwise, the handle is valid and points to a file/directory on disk.
	 * If 'path' is a symlink, 'h' points to its target and 'd' refers to the symlink itself. */
	i.exists = 1;
	i.size = _win_ulrg(d.nFileSizeHigh, d.nFileSizeLow);
	i.type = i_type_from_attr(d.dwFileAttributes);
	if (i.type != brrpath_type_reparse) {
		/* Path is not a reparse point */
		i.flags = i_flags_from_attr(d.dwFileAttributes);
	} else {
		/* Path is a reparse point, need to determine if symlink or otherwise */
		if (d.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT) {
			i.flags |= brrpath_flag_mount;
		} else if (d.dwReserved0 == IO_REPARSE_TAG_SYMLINK) {
			/* Reparse point is valid and is a symlink, query info for the target */
			BY_HANDLE_FILE_INFORMATION hi = {0};
			if (!GetFileInformationByHandle(h, &hi)) {
				/* Failed to query target */
				FindClose(h);
				brrapi_sete(BRRAPI_E_OSERR);
				return (brrpath_info_t){.size=BRRSZ_MAX};
			}
			/* Got the target info */
			i.size = _win_ulrg(hi.nFileSizeHigh, hi.nFileSizeLow);
			i.type = brrpath_type_symlink | (i_type_from_attr(hi.dwFileAttributes) << 4);
			i.flags = i_flags_from_attr(hi.dwFileAttributes);
		}
	}
	FindClose(h);
	/* Success! */
#endif
	return i;
}

brrpath_info_t BRRCALL
brrpath_fstat(brrpath_fd_t fd)
{
	brrpath_info_t i = {.exists=1};
#if brrplat_unix
	struct stat s = {0};
	if (fstat(fd, &s)) {
		brrapi_sete(errno == EBADF ? BRRAPI_E_ARGERR : BRRAPI_E_OSERR);
		return (brrpath_info_t){.size=BRRSZ_MAX};
	}
	i.size = s.st_size;
	i.type = i_type_from_attr(s.st_mode);
	i.flags = i_flags_from_attr(s.st_mode);
#elif brrplat_dos
	BY_HANDLE_FILE_INFORMATION hi = {0};
	if (!GetFileInformationByHandle(fd, &hi)) {
		brrapi_sete(BRRAPI_E_OSERR);
		return (brrpath_info_t){.size=BRRSZ_MAX};
	}
	i.size = _win_ulrg(hi.nFileSizeHigh, hi.nFileSizeLow);
	i.type = i_type_from_attr(hi.dwFileAttributes);
	i.flags = i_flags_from_attr(hi.dwFileAttributes);
#endif
	return i;
}
