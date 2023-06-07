/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */
#include "brrtools/brrapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#if brrplat_unix
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#else
#include <Windows.h>
#endif

#include "brrtools/brrfile.h"

typedef struct brrfd {
	brrstat_t st;
	char *path; /* path string */

	char *b; /* read buffer data */
	brrbsize_t buf_size; /* buffer size */
	brrbsize_t buf_fill; /* buffer fill, how much of the buffer is valid data read from the file */
	brrbpos_t bpos; /* buffer position */
	brrfpos_t fpos;
	brrfd_mode_t file_mode;
	int buf_invalidate; /* buffer needs re-read if we try to read from it again */
#if brrplat_unix
	int fd;
#else
	FILE *fp;
#endif
} brrfd_t;

#if brrplat_dos
#define windows_imp() do { \
	fprintf(stderr, "PLEASE IMPLEMENT WINDOWS %s\n", __func__);\
	abort();
} while (0);
#else
#define windows_imp()
#endif

int
brrfd_check(const brrfd_t *const fd)
{
	if (!fd || !fd->path || !fd->b)
		return -1;
#if brrplat_unix
	if (fd->fd == -1)
		return -1;
#else
	windows_imp();
#endif
	return 0;
}

void
brrfd_close(brrfd_t *const fd)
{
	if (!fd)
		return;
#if brrplat_unix
	close(fd->fd);
#else
	windows_imp();
#endif
	if (fd->path)
		BRRAPI_FREE(fd->path);
	if (fd->b)
		BRRAPI_FREE(fd->b);
	memset(fd, 0, sizeof(*fd));
#if brrplat_unix
	fd->fd = -1;
#else
#endif
	BRRAPI_FREE(fd);
}

brrfd_t *
brrfd_open(const char *const path, brrfd_mode_t mode, brrfpos_t fpos, brrbsize_t buffer_size)
{
	if (!path) {
		brrapi_error(BRRAPI_E_ARGERR, "brrfd_open given NULL path");
		return NULL;
	}
	if (buffer_size && buffer_size < BRRBUFFER_MINIMUM_SIZE) {
		brrapi_error(BRRAPI_E_ARGERR, "brrfd_open Insufficient buffer size, must be at least %u bytes", buffer_size);
		return NULL;
	}
	if (buffer_size > BRRBUFFER_MAXIMUM_SIZE) {
		buffer_size = BRRBUFFER_MAXIMUM_SIZE;
	}
	int is_read = mode & brrfdf_r;
	int is_write = mode & brrfdf_w;
	int is_read_only = is_read && !is_write;
	int is_write_only = !is_read && is_write;
	if (!is_read && !is_write) {
		brrapi_error(BRRAPI_E_ARGERR, "brrfd_open given no mode to open '%s' with; please specify read, write, or read/write");
		return NULL;
	}

	 brrstat_t st; {
		if (brrstat(&st, path)) {
			brrapi_error(BRRAPI_E_APPEND_CODE, "brrfd_open failed");
			return NULL;
		} else if (st.ltype == brrstat_ltype_none && is_read_only) {
			brrapi_error(BRRAPI_E_ARGERR, "brrfd_open Can't open '%s' for reading, doesn't exist", path);
			return NULL;
		} else if (st.ftype != brrstat_ftype_file) {
			brrapi_error(BRRAPI_E_ARGERR, "brrfd_open Can't open '%s', is not a file", path);
			return NULL;
		}
	}
	brrfd_t *r = BRRAPI_ALLOC(sizeof(*r));
	if (!r) {
		brrapi_error(BRRAPI_E_MEMERR, "brrfd_open Failed to allocate space for '%s'", path);
		return NULL;
	}
	memset(r, 0, sizeof(*r));
	r->st = st;

	int l = strlen(path);
	if (!(r->path = BRRAPI_ALLOC(l + 1))) {
		brrfd_close(r);
		brrapi_error(BRRAPI_E_MEMERR, "brrfd_open Failed to allocate space for '%s'", path);
		return NULL;
	}
	memcpy(r->path, path, l);
	r->path[l] = 0;

	if (buffer_size) {
		if (!(r->b = BRRAPI_ALLOC(buffer_size))) {
			brrfd_close(r);
			brrapi_error(BRRAPI_E_MEMERR, "brrfd_open Failed to allocate %d bytes for memory buffer of '%s'", buffer_size, path);
			return NULL;
		}
		memset(r->b, 0, buffer_size);
		r->buf_size = buffer_size;
	}

#if brrplat_unix

	int flags = 0;
	if (is_read && is_write) flags = O_RDWR;
	else if (is_read) flags = O_RDONLY;
	else flags = O_WRONLY;

	if ((mode & brrfdf_truncate) && is_write)
		flags |= O_TRUNC;
	if (r->st.ltype == brrstat_ltype_none && is_write)
		flags |= O_CREAT;

	if (-1 == (r->fd = open(path, flags, 0x664))) {
		brrfd_close(r);
		brrapi_error(BRRAPI_E_IOERR, "brrfd_open failed to open '%s'", path);
		return NULL;
	}

	int64_t i = INT64_MAX / 2;
	int when = (mode & brrfdf_append) ? SEEK_END : SEEK_SET;
	if (-1 == lseek(r->fd, fpos, when)) {
		brrfd_close(r);
		brrapi_error(BRRAPI_E_IOERR, "brrfd_open failed to seek to offset %"PRId64" of '%s'", fpos, path);
		return NULL;
	}

#else
	windows_imp();
#endif

	r->file_mode = mode;
	return r;
}

brrstat_t
brrfd_stat(const brrfd_t *const fd)
{
	if (!fd)
		return (brrstat_t){0};
	return fd->st;
}

const char *
brrfd_path(const brrfd_t *const fd)
{
	if (!fd)
		return NULL;
	return fd->path;
}

brrfpos_t
brrfd_pos(const brrfd_t *const fd)
{
	if (brrfd_check(fd))
		return -1;
	return fd->fpos + (fd->b ? fd->bpos : 0);
}

static inline brrfpos_t
i_read(brrfd_t *const fd, char *dst, brrfsize_t size, brrfpos_t ofs)
{
	brrfpos_t r = 0;
#if brrplat_unix
	r = read(fd->fd, (char *)dst, size - r);
	if (r == -1) {
		brrapi_error(BRRAPI_E_IOERR, "Failed to read %zu bytes from '%s'", size, fd->path);
		return -1;
	} else if (r == 0) {
		brrapi_error(BRRAPI_E_IOERR, "EOF while reading '%s'", size, fd->path);
		return 0;
	}
	fd->fpos += r;
#else
	windows_imp();
#endif
	return r;
}
static inline brrfpos_t
i_update_buffer(brrfd_t *const fd)
{
	brrfpos_t r = i_read(fd, fd->b, fd->buf_size, 0);
	if (r == -1)
		return -1;
	fd->buf_fill = r;
	return r;
}

static inline brrfpos_t
i_read_buffer(brrfd_t *const fd, char *dst, brrfpos_t len, brrfpos_t ofs)
{
	if (!len)
		return 0;
	memcpy(dst + ofs, fd->b + fd->bpos, len);
	fd->bpos += len;
	return len;
}

brrfpos_t
brrfd_read(brrfd_t *restrict const fd, void *restrict const dst, brrfsize_t size)
{
	if (brrfd_check(fd) || !dst)
		return -1;
	if (!(fd->file_mode & brrfdf_r)) {
		brrapi_error(BRRAPI_E_ARGERR, "Tried to read from file opened in write-only mode '%s'", fd->path);
		return -1;
	}
	if (size <= 0)
		return 0;
	if (!fd->b || !fd->buf_size) {
		brrfpos_t r = 0;
		if (-1 == (r = i_read(fd, dst, size, 0)))
			return -1;
		return r;
	} else if (size > fd->buf_size) {
		/* read the rest of the buffer into dst, read rest of size into dst, then update buffer */
		brrfpos_t r = 0;
		if (fd->buf_fill && !fd->buf_invalidate) {
			brrfpos_t remain = fd->buf_fill - fd->bpos;
			i_read_buffer(fd, dst, remain, 0);
			r += remain;
		}
		brrfpos_t rs = i_read(fd, dst, size - r, r);
		if (rs == -1)
			return -1;
		r += rs;
		if (i_update_buffer(fd)) {
			return -1;
		}
		return r;
	} else {
		brrfpos_t result = 0;
		brrfpos_t remain = fd->buf_fill - fd->bpos;
		if (size > remain) {
			/* need to split the read into a copy, a read, and then a copy */
			i_read_buffer(fd, dst, remain, 0);
			if (i_update_buffer(fd)) {
				return -1;
			}
			i_read_buffer(fd, dst, size - remain, remain);
		} else {
			memcpy(dst, fd->b + fd->bpos, size);
			fd->bpos += size;
			result = size;
		}
		return result;
	}
}


brrfpos_t
brrfd_write(brrfd_t *const fd, const void *const src, brrfsize_t size)
{
	if (brrfd_check(fd))
		return -1;
	if (!(fd->file_mode & brrfdf_w)) {
		brrapi_error(BRRAPI_E_ARGERR, "Tried to write to file opened in read-only mode '%s'", fd->path);
		return -1;
	}
#if brrplat_unix
	brrfpos_t r = write(fd->fd, src, size);
	if (-1 == r) {
		brrapi_error(BRRAPI_E_IOERR, "Couldn't write %zu bytes to '%s'", size, fd->path);
		return -1;
	}
	return r;
#else
	windows_imp();
#endif
	return 0;
}

static inline int
i_seek(brrfd_t *const fd, brrfd_side_t side, brrfpos_t ofs) {
#if brrplat_unix
	int whence = side == brrfd_side_start ? SEEK_SET : (side == brrfd_side_current ? SEEK_CUR : SEEK_END);
	if (-1 == lseek(fd->fd, ofs, whence)) {
		brrapi_error(BRRAPI_E_IOERR, "Failed to seek file '%s'", fd->path);
		return -1;
	}
#elif brrplat_dos
	windows_imp()
#endif
	return 0;
}

int
brrfd_seek(brrfd_t *const fd, brrfd_side_t side, brrfpos_t ofs)
{
	if (brrfd_check(fd))
		return -1;
	if (side < 0 || side > 2) {
		brrapi_error(BRRAPI_E_ARGERR, "Invalid side for brrfd_seek %i", side);
		return -1;
	}
#if brrplat_unix
	brrfpos_t new_pos;
	if (side == brrfd_side_current) {
		if (ofs == 0)
			return 0;
		if (ofs < 0) {
			brrfsize_t o = -ofs;
			if (o <= fd->bpos) {
				fd->bpos -= o;
				return 0;
			} else if (o <= fd->fpos + fd->bpos) {
				new_pos = fd->fpos + fd->bpos - o;
			} else {
				new_pos = 0;
			}
		} else {
			brrbpos_t remain = fd->buf_fill - fd->bpos;
			if (ofs <= remain) {
				fd->bpos += ofs;
				return 0;
			} else {
				new_pos = fd->fpos + fd->bpos + ofs;
			}
		}
	} else if (side == brrfd_side_start) {
		if (ofs <= 0) {
			/* rewind */
			new_pos = 0;
		} else if (ofs >= fd->st.size) {
			/* seek to end */
			new_pos = fd->st.size;
		} else {
			new_pos = ofs;
		}
	} else {/* side == brrfd_side_end */
		if (ofs >= 0) {
			/* seek to end */
			new_pos = fd->st.size;
		} else if ((-ofs) >= fd->st.size) {
			/* rewind */
			new_pos = 0;
		} else {
			new_pos = fd->st.size - ofs;
		}
	}
	if (i_seek(fd, brrfd_side_start, new_pos))
		return -1;
	fd->buf_invalidate = 1;
#elif brrplat_dos
	windows_imp()
#endif
	return 0;
}

int
brrfd_invalidate(brrfd_t *const fd)
{
	if (brrfd_check(fd))
		return -1;
	if (i_seek(fd, brrfd_side_start, fd->fpos + fd->bpos))
		return -1;
	fd->buf_invalidate = 1;
	return 0;
}
