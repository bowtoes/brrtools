#ifndef BRRMETA_H
#define BRRMETA_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrtil.h>

BRRCPPSTART

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
} brrmeta_prop_typeT;

typedef struct brrmeta_prop_type_info {
	const char *name; /* Display name. */
	brru4 id; /* TBD */
} brrmeta_prop_type_infoT;

#define _brrmeta_prop_type_info_create0(_type_name_, _prop_type_) \
    ((brrmeta_prop_type_infoT){.name=_type_name_,.id=_prop_type_})
#define brrmeta_prop_type_info_create(_type_, _prop_type_) \
    _brrmeta_prop_type_info_create0(#_type_, (_prop_type_))

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
	const char *name; /* Display name. */
	brrsz offset; /* Offset position of property in struct, in bytes. */
	brrmeta_prop_type_infoT type; /* Type information of property. */
} brrmeta_propT;

#define _brrmeta_property_new0(_n_, _o_, _i_) ((brrmeta_propT){.name=_n_,.offset=_o_,.type=_i_})
#define brrmeta_property_new(_t_, _f_, _p_) \
    _brrmeta_property_new_impl(BRRTIL_AS_STRING(_f_), BRRTILL_GET_OFFSET(_t_, _f_), (_p_))

typedef struct brrmeta_struct_info {
	const char *name; /* Display name. */
	brru4 property_count; /* Length of properties list. */
	brrmeta_propT *properties; /* List of properties for a struct. */
} brrmeta_struct_infoT;

typedef struct brrmeta_struct_reg {
	struct {
		brru8 key;
		brrmeta_struct_infoT struct_info;
	} *registry;
	brru4 registry_length;
} brrmeta_regT;

typedef struct brrmeta_struct_decl {
	brrmeta_propT *properties; /* Properties list for this struct */
	brrsz size; /* Size of properties list in bytes */
} brrmeta_struct_declT;

/* Registers a new struct with name 'type_name' and decl 'decl' in registry
 * 'reg'.
 * If 'reg', 'type_name' or 'decl' are NULL, nothing is done and 0 is returned.
 * If an error occurs, nothing is done and -1 is returned.
 * Otherwise, the hash id of the new struct_info is returned.
 * */
BRRAPI brru8 BRRCALL _brrmeta_reg_struct_impl(brrmeta_regT *const reg,
    const char *type_name, const brrmeta_struct_declT *const decl);

BRRAPI int BRRCALL brrmeta_reg_new(brrmeta_regT *const reg);
#define brrmeta_reg_register(_reg_, _t_, _decl_) \
    _brrmeta_reg_struct_impl((_reg_), BRRTIL_AS_STRING(_t_), _decl_)
#define brrmeta_reg_get_info(_reg_, _t_) \
   _brrmeta_reg_get_info_impl((_reg_), BRRTIL_AS_STRING(_t_))

#define brrmeta_reg_get_value(_s_, _t_, _p_) \
    ((_t_ *)((brru1 *)(_s_) + (_p_)->offset))

BRRAPI brrmeta_struct_infoT *BRRCALL _brrmeta_reg_get_info_impl(const brrmeta_regT *const registry,
    const char *const type_name);

BRRAPI void BRRCALL brrmeta_reg_delete(brrmeta_regT *const reg);

BRRCPPEND

#endif /* BRRMETA_H */
