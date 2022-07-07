#ifndef BRRMETA_H
#define BRRMETA_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* TODO this definitely needs some work, please don't use. */

#define _brrmeta_string(_x_) #_x_

typedef enum brrmeta_prop_type {
	brrmeta_prop_type_u1 = 0,
	brrmeta_prop_type_u2,
	brrmeta_prop_type_u4,
	brrmeta_prop_type_u8,
	brrmeta_prop_type_s1,
	brrmeta_prop_type_s2,
	brrmeta_prop_type_s4,
	brrmeta_prop_type_s8,
	brrmeta_prop_type_f4,
	brrmeta_prop_type_f8,
	brrmeta_prop_type_str,
	brrmeta_prop_type_count
} brrmeta_prop_type_t;

typedef struct brrmeta_prop_type_info {
	const char *name; /* Type name */
	brru4 id;         /* TBD */
} brrmeta_prop_type_info_t;

#define _brrmeta_prop_type_info_create0(_type_name_, _prop_type_) \
    ((brrmeta_prop_type_info_t){.name=(_type_name_),.id=(_prop_type_)})
#define brrmeta_prop_type_info_create(_type_, _prop_type_) \
    _brrmeta_prop_type_info_create0(_brrmeta_string(_type_), (_prop_type_))

#define BRRMETA_TYPE_INFO_U1  brrmeta_prop_type_info_create(brru1,  brrmeta_prop_type_u1)
#define BRRMETA_TYPE_INFO_U2  brrmeta_prop_type_info_create(brru2,  brrmeta_prop_type_u2)
#define BRRMETA_TYPE_INFO_U4  brrmeta_prop_type_info_create(brru4,  brrmeta_prop_type_u4)
#define BRRMETA_TYPE_INFO_U8  brrmeta_prop_type_info_create(brru8,  brrmeta_prop_type_u8)
#define BRRMETA_TYPE_INFO_S1  brrmeta_prop_type_info_create(brrs1,  brrmeta_prop_type_s1)
#define BRRMETA_TYPE_INFO_S2  brrmeta_prop_type_info_create(brrs2,  brrmeta_prop_type_s2)
#define BRRMETA_TYPE_INFO_S4  brrmeta_prop_type_info_create(brrs4,  brrmeta_prop_type_s4)
#define BRRMETA_TYPE_INFO_S8  brrmeta_prop_type_info_create(brrs8,  brrmeta_prop_type_s8)
#define BRRMETA_TYPE_INFO_F4  brrmeta_prop_type_info_create(float,  brrmeta_prop_type_f4)
#define BRRMETA_TYPE_INFO_F8  brrmeta_prop_type_info_create(double, brrmeta_prop_type_f8)
#define BRRMETA_TYPE_INFO_STR brrmeta_prop_type_info_create(char *, brrmeta_prop_type_str)

typedef struct brrmeta_prop {
	const char *name;              /* Property name */
	brrsz offset;                  /* Offset position of property in struct, in bytes */
	brrmeta_prop_type_info_t type; /* Property type info */
} brrmeta_prop_t;

#define _brrmeta_property_new0(_n_, _o_, _i_) ((brrmeta_prop_t){.name=_n_,.offset=_o_,.type=_i_})
#define brrmeta_property_new(_t_, _f_, _p_) \
    _brrmeta_property_new0(_brrmeta_string(_f_), offsetof(_t_, _f_), (_p_))

typedef struct brrmeta_struct_info {
	const char *name;           /* Struct name */
	brrsz property_count;       /* Number of struct properties */
	brrmeta_prop_t *properties; /* Struct member property_info list */
} brrmeta_struct_info_t;

typedef struct brrmeta_struct_registry {
	struct {
		brru8 key;
		brrmeta_struct_info_t struct_info;
	} *registry;
	brrsz registry_length;
} brrmeta_registry_t;

typedef struct brrmeta_struct_decl {
	brrmeta_prop_t *properties; /* Properties list for this struct */
	brrsz size;                 /* Size of properties list in bytes */
} brrmeta_struct_decl_t;

BRRAPI int BRRCALL brrmeta_reg_new(brrmeta_registry_t *const reg);
BRRAPI void BRRCALL brrmeta_reg_free(brrmeta_registry_t *const reg);

/* Registers a new struct with name 'type_name' and decl 'decl' in registry
 * 'reg'.
 * If 'reg', 'type_name' or 'decl' are NULL, nothing is done and 0 is returned.
 * If an error occurs, nothing is done and -1 is returned.
 * Otherwise, the hash id of the new struct_info is returned.
 * */
BRRAPI brru8 BRRCALL _brrmeta_reg_struct_impl(brrmeta_registry_t *const regstry, const char *type_name, const brrmeta_struct_decl_t *const decl);
#define brrmeta_registry_register(_reg_, _t_, _decl_) \
    _brrmeta_registry_struct_impl((_reg_), _brrmeta_string(_t_), _decl_)

BRRAPI brrmeta_struct_info_t *BRRCALL _brrmeta_reg_get_info_impl(const brrmeta_registry_t *const registry, const char *const type_name);
#define brrmeta_registry_get_info(_reg_, _t_) \
   _brrmeta_registry_get_info_impl((_reg_), _brrmeta_string(_t_))

#define brrmeta_reg_get_value(_s_, _t_, _p_) \
    ((_t_ *)((brru1 *)(_s_) + (_p_)->offset))


#if defined(__cplusplus)
}
#endif

#endif /* BRRMETA_H */
