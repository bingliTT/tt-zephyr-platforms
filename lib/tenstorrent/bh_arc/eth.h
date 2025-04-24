/*
 * Copyright (c) 2024 Tenstorrent AI ULC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ETH_H
#define ETH_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <arc_dma.h>

#define MAX_ETH_INSTANCES 14

#define ETH_LIVE_STATUS_ADDR 0x7CE00
#define ETH_MAILBOX_ADDR 0x7D000
#define CMFW_MAILBOX_ID 2
#define ETH_CMFW_MAILBOX_ADDR (ETH_MAILBOX_ADDR + (CMFW_MAILBOX_ID * sizeof(eth_mailbox_t)))

#define ETH_MSG_STATUS_MASK 0xFFFF0000
#define ETH_MSG_CALL 0xCA110000
#define ETH_MSG_DONE 0xD0E50000

#define ETH_MSG_TYPE_MASK 0x0000FFFF
#define ETH_MSG_LINK_STATUS_CHECK 0x0001
#define ETH_MSG_RELEASE_CORE 0x0002

typedef struct {
	uint32_t msg;		/* 0 - Message type */
	uint32_t arg[3];	/* 1-3 - Arguments to the message (not all need to be used) */
} eth_mailbox_t;

typedef struct {
	uint32_t retrain_count;		/* 0 */
	uint32_t rx_link_up;		/* 1 - MAC/PCS RX Link Up */

	uint32_t spare[8-2];		/* 2-7 */

	/* Snapshot registers */
	uint64_t frames_txd;		/* 8,9 - Cumulative TX Packets Transmitted count */
	uint64_t frames_txd_ok;		/* 10,11 - Cumulative TX Packets Transmitted OK count */
	uint64_t frames_txd_badfcs;	/* 12,13 - Cumulative TX Packets Transmitted with BAD FCS */
	uint64_t bytes_txd;		/* 14,15 - Cumulative TX Bytes Transmitted count */
	uint64_t bytes_txd_ok;		/* 16,17 - Cumulative TX Bytes Transmitted OK count */
	uint64_t bytes_txd_badfcs;	/* 18,19 - Cumulative TX Bytes Transmitted with BAD FCS */
	uint64_t frames_rxd;		/* 20,21 - Cumulative Packets Received count */
	uint64_t frames_rxd_ok;		/* 22,23 - Cumulative Packets Received OK count */
	uint64_t frames_rxd_badfcs;	/* 24,25 - Cumulative Packets Received with BAD FCS count */
	uint64_t frames_rxd_dropped;	/* 26,27 - Cumulative Dropped Packets Received count */
	uint64_t bytes_rxd;		/* 28,29 - Cumulative Bytes received count */
	uint64_t bytes_rxd_ok;		/* 30,31 - Cumulative Bytes received OK count */
	uint64_t bytes_rxd_badfcs;	/* 32,33 - Cumulative Bytes received with BAD FCS count */
	uint64_t bytes_rxd_dropped;	/* 34,35 - Cumulative Bytes received and dropped count */
	uint64_t corr_cw;		/* 36,37 - Cumulative Corrected Codeword count */
	uint64_t uncorr_cw;		/* 38,39 - Cumulative Uncorrected Codeword count */

	uint32_t spare2[64 - 40];	/* 40-63 */
} eth_live_status_t;

void SetupEthSerdesMux(uint32_t eth_enabled);
void ReleaseEthReset(uint32_t eth_inst, uint32_t ring);

/**
 * @brief Load the ETH FW configuration data into ETH L1 memory
 * @param eth_inst ETH instance to load the FW config for
 * @param ring Load over NOC 0 or NOC 1
 * @param fw_image Pointer to the FW data
 * @param fw_size Size of the FW data
 * @return int 0 on success, -1 on failure
 */
int LoadEthFw(uint32_t eth_inst, uint32_t ring, uint8_t *fw_image, uint32_t fw_size);

/**
 * @brief Load the ETH FW configuration data into ETH L1 memory
 * @param eth_inst ETH instance to load the FW config for
 * @param ring Load over NOC 0 or NOC 1
 * @param eth_enabled Bitmask of enabled ETH instances
 * @param fw_cfg_image Pointer to the FW config data
 * @param fw_cfg_size Size of the FW config data
 * @return int 0 on success, -1 on failure
 */
int LoadEthFwCfg(uint32_t eth_inst, uint32_t ring, uint32_t eth_enabled,
	uint8_t *fw_cfg_image, uint32_t fw_cfg_size);

/**
 * @brief Send a message to the ETH mailbox, waits for mailbox availability
 * @param ring Load over NOC 0 or NOC 1
 * @param msg uint32_t message type to send, see eth.h for message types under ETH_MSG
 * @param arg0 uint32_t first argument to send with the message
 * @param arg1 uint32_t second argument to send with the message
 * @param arg2 uint32_t third argument to send with the message
 * @param timeout_ms uint32_t how long to wait for the message to be serviced before returning
 * @return int whether the message successfully sent or not
 */
int sendEthMessage(uint32_t eth_inst, uint32_t ring, uint32_t msg, uint32_t arg0,
	uint32_t arg1, uint32_t arg2, uint32_t timeout_ms);

/**
 * @brief Get the ETH live status by sending a message to the ETH mailbox
 * @param eth_inst ETH instance to get the live status for
 * @param ring Load over NOC 0 or NOC 1
 * @param eth_live_status Pointer to the eth_live_status_t struct to fill with the live status
 * @param timeout_ms uint32_t how long to wait for the message to be serviced before returning
 * @return int whether the message successfully sent or not
 */
int getEthLiveStatus(uint32_t eth_inst, uint32_t ring, eth_live_status_t *eth_live_status,
	uint32_t timeout_ms);

#endif
