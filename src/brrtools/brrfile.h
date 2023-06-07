/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrfile_h
#define brrfile_h

#include <stdint.h>

#include <brrtools/brrstat.h>

typedef enum brrfd_mode {
	brrfdf_r        = 0x01, /* Open for reading exclusively */
	brrfdf_w        = 0x02, /* Open for writing exclusively */
	brrfdf_rw       = 0x03,

	brrfdf_truncate = 0x04, /* When opening for writing, don't trunctate the contents of the given file. */
	brrfdf_append   = 0x08, /* When opening for writing, seek to the end of the file when openening. */
} brrfd_mode_t;

#define brrfd_mode2(_1_, _2_) ((brrfd_mode_##_1_) | (brrfd_mode_##_2_))
#define brrfd_mode3(_1_, _2_, _3_) (brrfd_mode2(_1_, _2_) | (brrfd_mode_##_3_))

typedef struct brrfd brrfd_t;

int
brrfd_check(const brrfd_t *const fd);

void
brrfd_close(brrfd_t *const fd);

brrfd_t *
brrfd_open(const char *const path, brrfd_mode_t mode, brrfpos_t pos_offset, brrbsize_t buffer_size);

brrstat_t
brrfd_stat(const brrfd_t *const fd);

const char *
brrfd_path(const brrfd_t *const fd);

brrfpos_t
brrfd_pos(const brrfd_t *const fd);

brrfpos_t
brrfd_read(brrfd_t *restrict const fd, void *restrict const dst, brrfsize_t size);

brrfpos_t
brrfd_write(brrfd_t *const fd, const void *const src, brrfsize_t size);

typedef enum brrfd_side {
	brrfd_side_start = 0,
	brrfd_side_current = 1,
	brrfd_side_end = 2
} brrfd_side_t;

int
brrfd_seek(brrfd_t *const fd, brrfd_side_t side, brrfpos_t ofs);

int
brrfd_invalidate(brrfd_t *const fd);

#endif /* brrfile_h */
