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

#ifndef BRRTOOLS_BRRAPI_H
#define BRRTOOLS_BRRAPI_H

#if defined(_WIN32)
# define BRRCALL __cdecl
# if defined(BRRTOOLS_EXPORTS) || defined(BRRTOOLS_IMPORTS)
#  if defined(BRRTOOLS_EXPORTS) && !defined(BRRTOOLS_IMPORTS)
#   define BRRAPI __declspec(dllexport)
#  else
#   define BRRAPI __declspec(dllimport)
#  endif // BRRTOOLS_EXPORTS && !BRRTOOLS_IMPORTS
# else // Static library
#  define BRRAPI
# endif // BRRTOOLS_EXPORTS || BRRTOOLS_IMPORTS
#else // UNIX target
# define BRRAPI
# define BRRCALL
#endif // BRRPLATFORMTYPE_WINDOWS

#if defined(__cplusplus)
# define BRRCPPSTART extern "C" {
# define BRRCPPEND }
#else
# define BRRCPPSTART
# define BRRCPPEND
#endif // __cplusplus

#endif /* BRRTOOLS_BRRAPI_H */
