/*
 * Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <plat/common/platform.h>

/* Allow platforms to override the log prefix string */
#pragma weak plat_log_get_prefix

//【改】在 INFO 和 VERBOSE 之间加上“ATF_BP”
// static const char *plat_prefix_str[] = {
// 	"ERROR:   ", "NOTICE:  ", "WARNING: ", "INFO:    ", "VERBOSE: "};

static const char *plat_prefix_str[] = {
	"ERROR:   ", "NOTICE:  ", "WARNING: ", "INFO:    ", "ATF_BP: ","VERBOSE: "};

const char *plat_log_get_prefix(unsigned int log_level)
{
	unsigned int level;

	if (log_level < LOG_LEVEL_ERROR) {
		level = LOG_LEVEL_ERROR;
	} else if (log_level > LOG_LEVEL_VERBOSE) {
		level = LOG_LEVEL_VERBOSE;
	} else {
		level = log_level;
	}

	return plat_prefix_str[(level / 10U) - 1U];
}
