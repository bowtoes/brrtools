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

#ifndef BRRPATH_H
#define BRRPATH_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrstr.h>

BRRCPPSTART

typedef enum brrpath_type {
	brrpath_type_invald    = 0x00,
	brrpath_type_file      = 0x01,
	brrpath_type_directory = 0x02,
	brrpath_type_link      = 0x04,
	brrpath_type_irregular = 0xF0,
} brrpath_typeT;

typedef enum brrpath_stat_code {
	brrpath_stat_success      = 0x00, // No stat error.
	brrpath_stat_invalid_type = 0x01, // Invalid path type for stat.
	brrpath_stat_invalid_path = 0x02, // Invalid path string.
	brrpath_stat_error        = 0xF0, // Unspecified stat error.
} brrpath_stat_codeT;

typedef enum brrpath_perms {
	brrpath_perms_none       = 0x00,
	brrpath_perms_readable   = 0x01,
	brrpath_perms_writable   = 0x02,
	brrpath_perms_executable = 0x04,
} brrpath_permsT;

typedef struct brrpath_stat {
	brru8 size;
	brrb1 exists;
	brrb1 is_absolute;
	brrpath_typeT type;
	brrpath_permsT perms;
	brrpath_stat_codeT error;
} brrpath_statT;

BRRAPI char *BRRCALL brrpath_flatten(const char *const path);

/* Determines if 'path' represents an absolute system path or not.
 * If 'path' is invalid (NULL or length == 0), false is returned.
 * */
BRRAPI brrb1 BRRCALL brrpath_is_absolute(const char *const path);

/* Gathers various information about the given path, optionally following the
 * symbolic link 'path' if it is one.
 * If an error occurs or 'path' is invalid, the returned struct 'error' will
 * contain flags indicating the type of error.
 * If statting succeeds, the returned struct 'error' will be 0.
 * */
BRRAPI brrpath_statT BRRCALL brrpath_stat(const char *const path, int follow_link);

/* Returns 'true' if 'path' is an existing path, false otherwise.
 * If 'path' is an invalid string (NULL or length == 0), returns false.
 * */
BRRAPI brrb1 BRRCALL brrpath_exists(const char *const path, int follow_link);

/* Returns the type of 'path'.
 * If 'path' is an invalid string (NULL or length == 0) or invalid path,
 * returns 'brrpath_irregular'.
 * If 'path' doesn't exist, returns 'brrpath_directory'.
 * */
BRRAPI brrpath_typeT BRRCALL brrpath_type(const char *const path, int follow_link);

BRRCPPEND

#endif /* BRRPATH_H */
