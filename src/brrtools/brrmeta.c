#include "brrtools/brrmeta.h"

#include <stdlib.h>
#include <string.h>

#include "brrtools/brrlib.h"
#include "brrtools/brrrand.h"

int BRRCALL
brrmeta_reg_new(brrmeta_registry_t *const registry)
{
	return 0;
}
void BRRCALL
brrmeta_regstry_delete(brrmeta_registry_t *const registry)
{
	if (registry) {
		for (brrsz i = 0; i < registry->registry_length; ++i) {
			brrlib_alloc((void **)&registry->registry[i].struct_info.properties, 0, 0);
		}
		brrlib_alloc((void **)&registry->registry, 0, 0);
		registry->registry = NULL;
		registry->registry_length = 0;
	}
}

brru8 BRRCALL
_brrmeta_reg_struct_impl(brrmeta_registry_t *const registry,
    const char *type_name, const brrmeta_struct_decl_t *const decl)
{
	brrmeta_struct_info_t struct_info = {0};
	brru4 prop_count = decl->size / sizeof(brrmeta_prop_t);
	brru8 key;
	if (!registry || !type_name || !decl)
		return 0;
	struct_info.name = type_name;
	struct_info.property_count = prop_count;

	if (brrlib_alloc((void **)&struct_info.properties, decl->size, 1)) {
		return -1;
	}

	memcpy(struct_info.properties, decl->properties, decl->size);
	key = brrrand_hash(type_name, strlen(type_name));

	if (brrlib_alloc((void **)&registry->registry, (registry->registry_length + 1) * sizeof(*registry->registry), 0)) {
		brrlib_alloc((void **)&struct_info.properties, 0, 0);
		return -1;
	}

	registry->registry[registry->registry_length].key = key;
	registry->registry[registry->registry_length].struct_info = struct_info;
	registry->registry_length++;
	return key;
}

brrmeta_struct_info_t *BRRCALL
_brrmeta_reg_get_info_impl(const brrmeta_registry_t *const registry,
    const char *const type_name)
{
	brrmeta_struct_info_t *info = NULL;
	brru8 hash;
	if (!registry || !type_name)
		return NULL;
	hash = brrrand_hash(type_name, strlen(type_name));
	for (brrsz i = 0; i < registry->registry_length; ++i) {
		if (registry->registry[i].key == hash) {
			info = &registry->registry[i].struct_info;
		}
	}
	return info;
}
