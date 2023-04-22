/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include <stdio.h>
#include <stdlib.h>

#include "brrtools/brrfile.h"

brrsz
brrfile_read(const char *const path, const brrpath_inf_t *const inf, void **const buffer)
{
	if (!buffer) {
		brrapi_error(BRRAPI_E_ARGERR, "brrfile_read given no buffer to read to");
		return BRRSZ_MAX;
	} else if (!path) {
		brrapi_error(BRRAPI_E_ARGERR, "brrfile_read given NULL path to read from");
		return BRRSZ_MAX;
	}

	brrpath_inf_t i;
	if (inf) {
		i = *inf;
	} else if (brrpath_stat(&i, path)) {
		brrapi_error(BRRAPI_E_APPEND_CODE, "Tried to stat '%s'", path);
		return BRRSZ_MAX;
	}

	if (!i.exists) {
		brrapi_error(BRRAPI_E_ARGERR, "Can't read from file '%s': Does not exist", path);
		return BRRSZ_MAX;
	} else if (i.type == brrpath_type_invalid || i.type == brrpath_type_dir || i.type == brrpath_type_device) {
		brrapi_error(BRRAPI_E_ARGERR, "Can't read from '%s': Is not a file", path);
		return BRRSZ_MAX;
	}

	const brrsz s = brrpath_get_size(i.size);

	char *b = malloc(s);
	if (!b) {
		brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate %zu bytes for reading '%s'", s, path);
		return BRRSZ_MAX;
	}
	FILE *f = fopen(path, "rb");
	if (!f) {
		free(b);
		brrapi_error(BRRAPI_E_OSERR, "Failed to open '%s' for reading", path);
		return BRRSZ_MAX;
	}

	const brrsz r = fread(b, 1, s, f);
	if (r < s) {
		if (ferror(f))
			brrapi_error(BRRAPI_E_OSERR, "Error reading '%s'", path);
	}
	fclose(f);
	*buffer = b;
	return r;
}

brrsz
brrfile_write(const char *const path, const brrpath_inf_t *const inf, const void *const buffer, brrsz size)
{
	if (!path) {
		brrapi_error(BRRAPI_E_ARGERR, "brrfile_write given NULL path to write to");
		return BRRSZ_MAX;
	} else if (!buffer) {
		brrapi_error(BRRAPI_E_ARGERR, "brrfile_write given NULL data to write to '%s'", path);
		return BRRSZ_MAX;
	} else if (inf) {
		if (inf->exists && !(inf->type == brrpath_type_file || inf->type == brrpath_type_device)) {
			brrapi_error(BRRAPI_E_ARGERR, "Can't write to path '%s': Isn't a file", path);
			return BRRSZ_MAX;
		}
	}

	FILE *f = fopen(path, "wb");
	if (!f) {
		brrapi_error(BRRAPI_E_OSERR, "Failed to open '%s' for writing", path);
		return BRRSZ_MAX;
	}
	const brrsz s = fwrite(buffer, 1, size, f);
	if (s < size) {
		if (ferror(f)) {
			brrapi_error(BRRAPI_E_OSERR, "Error writing '%s'", path);
		}
	}
	fclose(f);
	return s;
}
