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
/* A struct holding various information about a path on disk.
 * Fields:
 * brru8 size : Size of path in bytes; 0 if path is a directory.
 * brrpath_typeT type : Type of path.
 * brrstgT directory : The directory component of the path.
 * brrstgT base_name : The base name component of the path.
 * brrstgT full_path : The joining of 'directory' and 'base_name'.
 * brrstgT extension : The extension that is on the base name component of the path; if the path is a directory, this is empty.
 * brrsz depth : Depth that the path was found if it was constructed from 'brrpath_walk'.
 * int exists : Non-zero if the path exists on disk.
 * */
typedef struct brrpath_info {
	brru8 size; /* Size of path in bytes; 0 if path is a directory. */
	brrpath_typeT type; /* Type of path. */
	brrstgT full_path; /* The directory component of the path. */
	brrstgT directory; /* The base name component of the path. */
	brrstgT base_name; /* The joining of 'directory' and 'base_name'. */
	brrstgT extension; /* The extension that is on the base name component of the path; if the path is a directory, this is empty. */
	brrsz depth; /* Depth that the path was found if it was constructed from 'brrpath_walk'. */
	int exists; /* Non-zero if the path exists on disk. */
} brrpath_infoT;
/* Options passed to 'brrpath_walk'.
 * Fields:
 * - brrstgT path : Path to walk; if it is a file, the directory containing the file will be walked.
 * - brrsz min_depth : Minimum depth to include in result.
 * - brrsz max_depth : Depth to stop walking at.
 * */
typedef struct brrpath_walk_options {
	brrstgT path; /* Path to walk; if it is a file, the directory containing the file will be walked. */
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

/* Deletes all associated memory with 'info'.
 * If 'result' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrpath_info_delete(brrpath_infoT *const info);
/* Deletes all infos in 'result' and deletes the array. Sets 'result_count' to 0.
 * If 'result' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrpath_walk_result_delete(brrpath_walk_resultT *const result);
/* Stats 'path' and puts the results in 'st'.
 * If 'path' or 'st' is NULL, nothing is done and 0 is returned.
 * If 'path' is a NULL string, nothing is done and 1 is returned.
 *
 * */
BRRAPI int BRRCALL brrpath_stat(const brrstgT *const path, brrpath_stat_resultT *const st);
/* Walks all directories and subdirectories of 'options->path' according to the
 * fields specified in 'options', applying 'filter' to the results that are then
 * stored in 'result'; returns 0 on successful operation.
 * For all walked paths that 'filter' returns non-zero, they are added to 'result'.
 * If 'result' or 'options' is NULL, nothing is done and 0 is returned.
 * If 'options->path' is a NULL string, nothing is done and 1 is returned.
 * If 'options->path' does not exist, 'result' will be empty and 0 is returned.
 * If 'filter' is NULL, a default filter will be used.
 * If an error occurs, 'result' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrpath_walk(
    brrpath_walk_resultT *const result,
    const brrpath_walk_optionsT *const options,
    int (*BRRCALL filter)(const brrpath_infoT *const path_info));

BRRCPPEND

#endif /* BRRPATH_H */
