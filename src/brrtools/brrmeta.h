#ifndef BRRMETA_H
#define BRRMETA_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

typedef enum brrmeta_property_type {
	brrmeta_property_type_u1 = 0,
	brrmeta_property_type_u4,
	brrmeta_property_type_u8,
	brrmeta_property_type_s1,
	brrmeta_property_type_s4,
	brrmeta_property_type_s8,
	brrmeta_property_type_f4,
	brrmeta_property_type_f8,
	brrmeta_property_type_str,
	brrmeta_property_type_count
} brrmeta_property_typeT;

typedef struct brrmeta_property_type_info {
	const char *name; /* Display name */
	brru4 id;         /* TBD */
} brrmeta_property_type_infoT;

#endif /* BRRMETA_H */
