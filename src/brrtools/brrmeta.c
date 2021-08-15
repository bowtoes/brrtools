#include "brrtools/brrmeta.h"

#include <stdlib.h>
#include <string.h>

#include "brrtools/brrlib.h"

int BRRCALL
brrmeta_reg_new(brrmeta_regT *const reg)
{
	return 0;
}

brru8 BRRCALL
_brrmeta_reg_struct_impl(brrmeta_regT *const reg,
    const char *type_name, const brrmeta_struct_declT *const decl)
{
	brrmeta_struct_infoT struct_info = {0};
	brru4 prop_count = decl->size / sizeof(brrmeta_propT);
	brru8 key;
	if (!reg || !type_name || !decl)
		return 0;
	struct_info.name = type_name;
	struct_info.property_count = prop_count;

	if (brrlib_alloc((void **)&struct_info.properties, decl->size, 1)) {
		return -1;
	}

	memcpy(struct_info.properties, decl->properties, decl->size);
	key = brrlib_hash(type_name, strlen(type_name));

	if (brrlib_alloc((void **)&reg->registry, (reg->registry_length + 1) * sizeof(*reg->registry), 0)) {
		brrlib_alloc((void **)&struct_info.properties, 0, 0);
		return -1;
	}

	reg->registry[reg->registry_length].key = key;
	reg->registry[reg->registry_length].struct_info = struct_info;
	reg->registry_length++;
	return key;
}
brrmeta_struct_infoT *BRRCALL
_brrmeta_reg_get_info_impl(const brrmeta_regT *const registry,
    const char *const type_name)
{
	brrmeta_struct_infoT *info = NULL;
	brru8 hash;
	if (!registry || !type_name)
		return NULL;
	hash = brrlib_hash(type_name, strlen(type_name));
	for (brrsz i = 0; i < registry->registry_length; ++i) {
		if (registry->registry[i].key == hash) {
			info = &registry->registry[i].struct_info;
		}
	}
	return info;
}

void BRRCALL
brrmeta_reg_delete(brrmeta_regT *const reg)
{
	if (reg) {
		for (brrsz i = 0; i < reg->registry_length; ++i) {
			brrlib_alloc((void **)&reg->registry[i].struct_info.properties, 0, 0);
		}
		brrlib_alloc((void **)&reg->registry, 0, 0);
		reg->registry = NULL;
		reg->registry_length = 0;
	}
}
