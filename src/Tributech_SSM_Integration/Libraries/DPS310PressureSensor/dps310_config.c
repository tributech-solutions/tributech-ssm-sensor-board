/*
 * 	dps310_config.h *
 *  Created on: 2020 Aug 27
 *  Author: AEASCHER
 */

#include <DPS310PressureSensor/dps_register.h>
#include <DPS310PressureSensor/dps310_config.h>
#include "stdbool.h"
#include "stdint.h"

reg_mask_t config_registers[NUM_OF_COMMON_REGMASKS] = {
	{0x07, 0x70, 4}, // TEMP_MR
	{0x07, 0x07, 0}, // TEMP_OSR
	{0x06, 0x70, 4}, // PRS_MR
	{0x06, 0x07, 0}, // PRS_OSR
	{0x08, 0x07, 0}, // MSR_CTRL
	{0x09, 0x02, 1}, // FIFO_EN

	{0x08, 0x20, 5}, // TEMP_RDY
	{0x08, 0x10, 4}, // PRS_RDY
	{0x0A, 0x04, 2}, // INT_FLAG_FIFO
	{0x0A, 0x02, 1}, // INT_FLAG_TEMP
	{0x0A, 0x01, 0}, // INT_FLAG_PRS
};

reg_mask_t registers[DPS310_NUM_OF_REGMASKS] = {
	{0x0D, 0x0F, 0}, // PROD_ID
	{0x0D, 0xF0, 4}, // REV_ID
	{0x07, 0x80, 7}, // TEMP_SENSOR
	{0x28, 0x80, 7}, // TEMP_SENSORREC
	{0x09, 0x08, 3}, // TEMP_SE
	{0x09, 0x04, 2}, // PRS_SE
	{0x0C, 0x80, 7}, // FIFO_FL
	{0x0B, 0x01, 0}, // FIFO_EMPTY
	{0x0B, 0x02, 1}, // FIFO_FULL
	{0x09, 0x80, 7}, // INT_HL
	{0x09, 0x70, 4}, // INT_SEL
};

reg_block_t registerBlocks[2] = {
    {0x00, 3},
    {0x03, 3},
};
