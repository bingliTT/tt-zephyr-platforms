/*
 * Copyright (c) 2025 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>

#include <zephyr/ztest.h>
#include "eth.h"

ZTEST(eth_msg, test_get_status)
{
	/* Test sending a message to the ETH Mailbox */
	for (uint8_t eth_inst = 0; eth_inst < MAX_ETH_INSTANCES; eth_inst++) {
		eth_live_status_t eth_live_status;
		int retcode = getEthLiveStatus(eth_inst, 0, &eth_live_status, 1000);

		zassert_true(retcode == 0 || retcode == -EINVAL,
			"Failed to get ETH%02d live status: %d", eth_inst, retcode);
	}
}

ZTEST_SUITE(eth_msg, NULL, NULL, NULL, NULL, NULL);
