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

#ifndef BRRPATH_H
#define BRRPATH_H

#include <brrtools/brrapi.h>
#include <brrtools/brrplatform.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrstringr.h>

_brrcppstart

#define BRRPATH_WALK_ERROR_NONE 0
#define BRRPATH_WALK_ERROR_ARGUMENTS 1
#define BRRPATH_WALK_ERROR_RUNTIME -1

#if defined(BRRPLATFORMTYPE_Windows)
#include <windows.h>
# define _brrpath_ulrg(_l_, _h_) (((ULARGE_INTEGER){.u={.LowPart=(_l_),.HighPart=(_h_)}}).QuadPart)
/* TODO where does 256 come from? */
# define BRRPATH_MAX_NAME 256
# define BRRPATH_MAX_PATH (MAX_PATH - 1)
# define BRRPATH_SEP_CHR '\\'
# define BRRPATH_SEP_STR "\\"
# define BRRPATH_CASE_SENSITIVE 0
#elif defined(BRRPLATFORMTYPE_unix)
# define BRRPATH_MAX_NAME 255
/* FIXME not the same on all *nixes, limit in various headers */
# define BRRPATH_MAX_PATH 4095
# define BRRPATH_SEP_CHR '/'
# define BRRPATH_SEP_STR "/"
# define BRRPATH_CASE_SENSITIVE 1
#endif

/* Type of path.
 * Values:
 * - brrpath_type_none,      : Invalid/unset path type
 * - brrpath_type_file,      : Path is a file
 * - brrpath_type_directory, : Path is a directory
 * - brrpath_type_other,     : Path is some other type
 * */
typedef enum brrpath_type {
	brrpath_type_none = 0,  // Invalid/unset path type
	brrpath_type_file,      // Path is a file
	brrpath_type_directory, // Path is a directory
	brrpath_type_other,     // Path is some other type
} brrpath_type_t;
/* Result struct that holds basic information about a path.
 * Fields:
 * - brru8 size;          : Size of the path on disk in bytes; 0 if path doesn't exist or is a directory
 * - brrpath_type_t type; : The type of path specified; 0 if path doesn't exist
 * - brrbl exists;        : Whether the path exists on disk
 * */
typedef struct brrpath_stat_result {
	brru8 size;          // Size of the path on disk in bytes; 0 if path doesn't exist or is a directory
	brrpath_type_t type; // The type of path specified; 0 if path doesn't exist
	brrbl exists;        // Whether the path exists on disk
} brrpath_stat_result_t;

/* Stats 'path' on disk and stores the resulting information in 'st'.
 * Returns 0 on success.
 * If an error occurs or 'st' or 'path' are NULL, 'st' is unaffected and -1 is
 * returned.
 * Will not err if 'path' is non-existent.
 * */
BRRAPI int BRRCALL brrpath_stat(brrpath_stat_result_t *const st, const brrstringr_t *const path);

/* Cleans 'path' of all invalid characters and repeated path separators, storing the result in 'out'.
 * Returns 0 on success.
 * If an error occurs or 'path' or 'out' is NULL, 'out' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_clean(brrstringr_t *const out, const brrstringr_t *const path);

/* Listing for the distinct components of a path.
 * - brrstringr_t directory : The part of the path preceding the base_name
 * - brrstringr_t base_name : The base name of the last element of the path
 * - brrstringr_t extension : The part of the 'base_name' after the last '.', if there is one
 * */
typedef struct brrpath_components {
	brrstringr_t directory; // The part of the path preceding the base_name
	brrstringr_t base_name; // The base name of the last element of the path
	brrstringr_t extension; // The part of the 'base_name' after the last '.', if there is one
} brrpath_components_t;
/* Frees memory associated with 'components'.
 * Does nothing if 'components' is NULL.
 * */
BRRAPI void BRRCALL brrpath_components_free(brrpath_components_t *const components);
/* Splits 'path' into its directory, base name, and extension components (as 'directory/base_name.extension').
 * Any field left NULL will not have its component stored into it, i.e. leaving 'base_name' NULL will only set 'directory' and 'extension'.
 * However, if 'extension' is NULL and 'base_name' is not, then 'base_name' will be the full name of the file given, including the '.extension'.
 * Returns 0 on success.
 * If an error occurs or 'path' is NULL, 'directory', 'base_name', and 'extension' are left unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_split_parts(brrstringr_t *const directory, brrstringr_t *const base_name, brrstringr_t *const extension,
	const brrstringr_t *const path);
/* Splits 'path' into its directory, base name, and extension components, storing the splits in 'components'.
 * Returns 0 on success.
 * If an error occurs or 'path' or 'out' is NULL, 'components' is left unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_split(brrpath_components_t *const out, const brrstringr_t *const path);
/* Combines pathname components in 'components' into 'path' formatted as:
 *     (directory + '/') + base_name + ('.' + extension)
 * Any input component that is NULL will not be printed to 'path'.
 * Returns 0 on success.
 * If an error occurs or 'out' is NULL, 'path' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_join_parts(brrstringr_t *const out,
	const brrstringr_t *const directory,
	const brrstringr_t *const base_name,
	const brrstringr_t *const extension);
/* Combines pathname components in 'components' into 'path' formatted as:
 *     directory + '/' + base_name + '.' + extension
 * Returns 0 on success.
 * If an error occurs or 'out' or 'components' is NULL, 'out' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_join(brrstringr_t *const out, const brrpath_components_t *const components);

/* Extracts the directory component of 'path', and formats it in 'out' as:
 *     'directory/'
 * An empty string will be returned if there is no path separator in the input.
 * Returns 0 on success.
 * If an error occurs or 'path' or 'out' is NULL, 'out' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_directory(brrstringr_t *const out, const brrstringr_t *const path);
/* Extracts the base name component of 'path', without the directory component or extension.
 * I.e., 'directory/base_name.extension' -> 'base_name'
 * Returns 0 on success.
 * If an error occurs or 'path' or 'out' is NULL, 'out' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_base_name(brrstringr_t *const out, const brrstringr_t *const path);
/* Extracts the extension component of 'path', and formats it in 'out' as:
 *     '.extension'
 * An empty string will be returned if there is no dot '.' found after a path separator.
 * Returns 0 on success.
 * If an error occurs or 'path' or 'out' is NULL, 'out' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_extension(brrstringr_t *const out, const brrstringr_t *const path);

/* The primary struct holding various information about a path on disk.
 * Fields:
 * - brrstringr_t full_path;          : The pathname used the initialize the 'brrpath_info_t'
 * - brrpath_components_t components; : The components of 'full_path'
 * - brru8 size;                      : Size of path in bytes; 0 if path is a directory
 * - brrpath_type_t type;             : Depth that the path was found if it was constructed from 'brrpath_walk'
 * - brrbl exists;                    : Type of path; 0 if path doesn't exist
 * - brrsz depth;                     : Non-zero if the path exists on disk
 * */
typedef struct brrpath_info {
	brrstringr_t full_path;          // The pathname used the initialize the 'brrpath_info_t'
	brrpath_components_t components; // The components of 'full_path'
	brru8 size;                      // Size of path in bytes; 0 if path is a directory
	brrsz depth;                     // Depth that the path was found if it was constructed from 'brrpath_walk'
	brrpath_type_t type;             // Type of path; 0 if path doesn't exist
	brrbl exists;                    // Non-zero if the path exists on disk
} brrpath_info_t;
BRRAPI void BRRCALL brrpath_info_free(brrpath_info_t *const info);

typedef int (*BRRCALL brrpath_walk_filter_t)(const brrpath_info_t *);
/* Options passed to 'brrpath_walk'.
 * Fields:
 * - brrsz min_depth;              : Minimum depth to include in results
 * - brrsz max_depth;              : Minimum depth to stop walking
 * - brrpath_walk_filter_t filter; : Filter function that returns zero for paths that are to be included in the output
 * */
typedef struct brrpath_walk_options {
	/* TODO Add option for 'relative-to' so that output is relative to the given */
	brrsz min_depth;              // Minimum depth to include in result
	brrsz max_depth;              // Minimum depth to stop walking
	brrpath_walk_filter_t filter; // Filter function that returns zero for paths that are to be included in the output
} brrpath_walk_options_t;
/* The results from 'brrpath_walk', stored in an array.
 * Fields:
 * - brrpath_info_t *results; : An array containing the info of all paths walked and filtered
 * - brrsz n_results;         : Number of results in result array
 * */
typedef struct brrpath_walk_result {
	brrpath_info_t *results; // An array containing the info of all paths walked and filtered
	brrsz n_results;         // Number of results in result array
} brrpath_walk_result_t;

/* Walks all directories and subdirectories of 'path' according to 'options',
 * adding all files and directories to 'result' for which 'filter' returns
 * zero.
 * Returns 0 on success.
 * If an error occurs or 'result' or 'path' are NULL, 'result' is left
 * unaffected and -1 is returned.
 * A NULL filter can be used, in which case every encountered directory entry
 * is included in 'result'.
 * */
BRRAPI int BRRCALL brrpath_walk(brrpath_walk_result_t *const result,
    const brrstringr_t *const path, brrpath_walk_options_t options);
/* Deletes all infos in 'result' and deletes the array.
 * Sets 'n_results' to 0.
 * */
BRRAPI void BRRCALL brrpath_walk_result_free(brrpath_walk_result_t *const result);

_brrcppend

#endif /* BRRPATH_H */
