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
#include <brrtools/brrplatform.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrstg.h>

BRRCPPSTART

#define BRRPATH_WALK_ERROR_NONE 0
#define BRRPATH_WALK_ERROR_ARGUMENTS 1
#define BRRPATH_WALK_ERROR_RUNTIME -1

#if defined(BRRPLATFORMTYPE_WINDOWS)
#include <windows.h>
# define BRRPATH_MAX_NAME 256
# define BRRPATH_MAX_PATH (MAX_PATH - 1)
# define BRRPATH_SEP_CHR '\\'
# define BRRPATH_SEP_STR "\\"
#else
# define BRRPATH_MAX_NAME 255
/* TODO not the same on all *nixes, limit in headers */
# define BRRPATH_MAX_PATH 4095
# define BRRPATH_SEP_CHR '/'
# define BRRPATH_SEP_STR "/"
#endif

/* Type of path.
 * Values are:
 * - brrpath_type_none : Invalid/unset path type.
 * - brrpath_type_file : Path is a file.
 * - brrpath_type_directory : Path is a directory.
 * - brrpath_type_other : Path is some other type.
 * */
typedef enum brrpath_type {
	brrpath_type_none = 0x00, /* Invalid/unset path type. */
	brrpath_type_file = 0x01, /* Path is a file. */
	brrpath_type_directory = 0x02, /* Path is a directory. */
	brrpath_type_other = 0x04, /* Path is some other type. */
} brrpath_typeT;
/* Result struct that holds basic information about a path.
 * Fields:
 * - int exists : Non-zero if the path exists, 0 if it doesn't.
 * - brrpath_typeT type : The type of path specified, if it exists; 0 if path is a directory.
 * - brru8 size : Size of the path on disk in bytes if it exists.
 * */
typedef struct brrpath_stat_result {
	int exists; /* Non-zero if the path exists, 0 if it doesn't. */
	brrpath_typeT type; /* The type of path specified, if it exists; 0 if path is a directory. */
	brru8 size; /* Size of the path on disk in bytes if it exists. */
} brrpath_stat_resultT;
/* Listing for the distinct components of a path.
 * brrstgT directory : The part of the path preceding the base_name.
 * brrstgT base_name : The name of the path.
 * brrstgT extension : The part of the base_name after the last '.'.
 * */
typedef struct brrpath_components {
	brrstgT directory; /* The part of the path preceding the base_name; NULL if there is no path separator. */
	brrstgT base_name; /* The name of the path; never NULL and only empty if name begins with a period. */
	brrstgT extension; /* The extension to the name; NULL if there is no period in the name of the path. */
} brrpath_componentsT;
/* A struct holding various information about a path on disk.
 * Fields:
 * brrpath_componentsT components : Path components.
 * brru8 size : Size of path in bytes; 0 if path is a directory.
 * brrpath_typeT type : Type of path.
 * brrsz depth : Depth that the path was found if it was constructed from 'brrpath_walk'.
 * int exists : Non-zero if the path exists on disk.
 * */
typedef struct brrpath_info {
	brrstgT full_path;
	brrpath_componentsT components;
	brru8 size; /* Size of path in bytes; 0 if path is a directory. */
	brrpath_typeT type; /* Type of path. */
	int exists; /* Non-zero if the path exists on disk. */
	brrsz depth; /* Depth that the path was found if it was constructed from 'brrpath_walk'. */
} brrpath_infoT;
/* Options passed to 'brrpath_walk'.
 * Fields:
 * - brrstgT path : Path to walk; if it is a file, the directory containing the file will be walked.
 * - brrsz min_depth : Minimum depth to include in result.
 * - brrsz max_depth : Depth to stop walking at.
 * */
typedef struct brrpath_walk_options {
	brrsz min_depth; /* Minimum depth to include in result. */
	brrsz max_depth; /* Depth to stop walking at. */
	/* TODO Add option for 'relative-to' so that output is relative to the given. */
} brrpath_walk_optionsT;
/* The results from 'brrpath_walk'.
 * Fields:
 * - brrpath_infoT *walk_results : An array containing the info of all paths walked and filtered.
 * - result_count : Number of results in result array.
 * */
typedef struct brrpath_walk_result {
	brrpath_infoT *walk_results; /* An array containing the info of all paths walked and filtered. */
	brrsz result_count; /* Number of results in result array. */
} brrpath_walk_resultT;

/* Stats 'path' on disk and stores the resulting information in 'st' and returns 0.
 * If 'path' or 'st' is NULL, nothing is done and 0 is returned.
 * If 'path' is a NULL string, nothing is done and 1 is returned.
 * If an error occurs, 'st' will be empty and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_stat(brrpath_stat_resultT *const st, const char *const path);
/* Deletes all associated memory with 'info'.
 * If 'result' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrpath_info_delete(brrpath_infoT *const info);

/* Combines 'directory', 'base_name', and 'extension' into 'string' formatted like:
 * 'directory/' + 'base_name' + '.extension'
 * Any input segment is NULL, it will have it's output segment removed.
 * 0 is returned on success.
 * If 'directory', 'base_name', and 'extension' are NULL, 'string' will be empty.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * 'string' can be a NULL string.
 * */
BRRAPI int BRRCALL brrpath_combine(brrstgT *const string, const char *const directory, const char *const base_name, const char *const extension);
/* Splits 'path' into its directory, base name, and extension components.
 * and returns 0 on success.
 * If 'info' or 'path' are NULL, nothing is done and 0 is returned.
 * If an error occurs, 'info'
 * */
BRRAPI int BRRCALL brrpath_split(brrstgT *const directory, brrstgT *const base_name, brrstgT *const extension, const char *const path);

/* Walks all directories and subdirectories of 'path' according to the
 * fields specified in 'options', applying 'filter' to the results that are then
 * stored in 'result'; returns 0 on successful operation.
 * For all walked paths that 'filter' returns non-zero, they are added to 'result'.
 * If 'path' or 'result' or 'options' is NULL, nothing is done and 0 is returned.
 * If 'options->path' does not exist, 'result' will be empty and 0 is returned.
 * If 'filter' is NULL, a default filter will be used.
 * If an error occurs, 'result' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_walk(const char *const path,
    brrpath_walk_resultT *const result,
    const brrpath_walk_optionsT *const options,
    int (*BRRCALL filter)(const brrpath_infoT *const path_info));
/* Deletes all infos in 'result' and deletes the array. Sets 'result_count' to 0.
 * If 'result' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrpath_walk_result_delete(brrpath_walk_resultT *const result);

BRRCPPEND

#endif /* BRRPATH_H */
