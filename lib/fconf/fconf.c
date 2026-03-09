/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <common/debug.h>
#include <common/fdt_wrappers.h>
#include <lib/fconf/fconf.h>
#include <lib/fconf/fconf_dyn_cfg_getter.h>
#include <libfdt.h>
#include <plat/common/platform.h>
#include <platform_def.h>

int fconf_load_config(unsigned int image_id)
{
	int err;
	const struct dyn_cfg_dtb_info_t *config_info;

	assert((image_id == FW_CONFIG_ID) || (image_id == TB_FW_CONFIG_ID));
	if(image_id == FW_CONFIG_ID)
	{
		ATF_BP(" ------Loading FW_CONFIG in fconf_load_config [fconf]------\n");
	}
	else if(image_id == TB_FW_CONFIG_ID)
	{
		ATF_BP(" ------Loading TB_FW_CONFIG in fconf_load_config [fconf]------\n");
	}

	image_info_t config_image_info = {
		.h.type = (uint8_t)PARAM_IMAGE_BINARY,
		.h.version = (uint8_t)VERSION_2,
		.h.size = (uint16_t)sizeof(image_info_t),
		.h.attr = 0
	};


	ATF_BP(" START: FCONF_GET_PROPERTY(dyn_cfg, dtb, image_id)[image_id=%u] in fconf_load_config [fconf]\n", image_id);
	config_info = FCONF_GET_PROPERTY(dyn_cfg, dtb, image_id);
	ATF_BP(" FINISH: FCONF_GET_PROPERTY(dyn_cfg, dtb, image_id)[image_id=%u] in fconf_load_config [fconf]\n", image_id);

	assert(config_info != NULL);

	config_image_info.image_base = config_info->config_addr;
	config_image_info.image_max_size = config_info->config_max_size;

	ATF_BP(" START: load_auth_image[image_id=%u] in fconf_load_config [fconf]\n", image_id);

	VERBOSE("FCONF: Loading config with image ID: %u\n", image_id);
	err = load_auth_image(image_id, &config_image_info);
	if (err != 0) {
		VERBOSE("Failed to load config %u\n", image_id);
		return err;
	}
	ATF_BP(" FINISH: load_auth_image[image_id=%u] in fconf_load_config [fconf]\n", image_id);


	INFO("FCONF: Config file with image ID:%u loaded at address = 0x%lx\n",
	     image_id, config_image_info.image_base);

	return 0;
}

void fconf_populate(const char *config_type, uintptr_t config)
{
	assert(config != 0UL);
	ATF_BP(" START: fdt_check_header((void *)config) in fconf_populate [fconf]\n");

	/* Check if the pointer to DTB is correct */
	if (fdt_check_header((void *)config) != 0) {
		ERROR("FCONF: Invalid DTB file passed for %s\n", config_type);
		panic();
	}
	ATF_BP(" FINISH: fdt_check_header((void *)config) in fconf_populate [fconf]\n");

	INFO("FCONF: Reading %s firmware configuration file from: 0x%lx\n", config_type, config);

	/* Go through all registered populate functions */
	IMPORT_SYM(struct fconf_populator *, __FCONF_POPULATOR_START__, start);
	IMPORT_SYM(struct fconf_populator *, __FCONF_POPULATOR_END__, end);
	const struct fconf_populator *populator;

	for (populator = start; populator != end; populator++) {
		assert((populator->info != NULL) && (populator->populate != NULL));

		if (strcmp(populator->config_type, config_type) == 0) {
			INFO("FCONF: Reading firmware configuration information for: %s\n", populator->info);
			if (populator->populate(config) != 0) {
				/* TODO: handle property miss */
				panic();
			}
		}
	}
}
