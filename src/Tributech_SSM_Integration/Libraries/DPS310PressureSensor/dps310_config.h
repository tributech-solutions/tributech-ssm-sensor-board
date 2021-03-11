/*
 * 	dps310_config.h *
 *  Created on: 2020 Aug 13
 *  Author: AEASCHER
 */

#ifndef DSP310_PRESSURESENSOR_DPS310_CONFIG_H_
#define DSP310_PRESSURESENSOR_DPS310_CONFIG_H_

#include "dps_register.h"
#include "stdbool.h"
#include "stdint.h"

#define DPS310__OSR_SE 3U

// DPS310 has 10 milliseconds of spare time for each synchronous measurement / per second for asynchronous measurements
// this is for error prevention on friday-afternoon-products :D
// you can set it to 0 if you dare, but there is no warranty that it will still work
#define DPS310__BUSYTIME_FAILSAFE 10U
#define DPS310__MAX_BUSYTIME ((1000U - DPS310__BUSYTIME_FAILSAFE) * DPS310_BUSYTIME_SCALING)


// these are 7 bit addresses
#define DPS310_ADDRESS_SDO_FLOAT		0x77U
#define DPS310_ADDRESS_SDO_GND			0x76U

#define DPS310_FIFO_SIZE 				32
#define DPS310_RESULT_BLOCK_LENGTH		3
#define NUM_OF_COMMON_REGMASKS			16
#define DPS310_NUM_OF_REGMASKS 			16

#define DPS310_PRESCISSION_1x			0
#define DPS310_PRESCISSION_2x			1
#define DPS310_PRESCISSION_4x			2
#define DPS310_PRESCISSION_8x			3
#define DPS310_PRESCISSION_16x			4
#define DPS310_PRESCISSION_32x			5
#define DPS310_PRESCISSION_64x			6
#define DPS310_PRESCISSION_128x			7

#define DPS310_OVERSAMPLING_RATE_1 		0
#define DPS310_OVERSAMPLING_RATE_2 		1
#define DPS310_OVERSAMPLING_RATE_4 		2
#define DPS310_OVERSAMPLING_RATE_8 		3
#define DPS310_OVERSAMPLING_RATE_16 	4
#define DPS310_OVERSAMPLING_RATE_32 	5
#define DPS310_OVERSAMPLING_RATE_64 	6
#define DPS310_OVERSAMPLING_RATE_128 	7

//we use 0.1 ms units for time calculations, so 10 units are one millisecond
#define DPS310_BUSYTIME_SCALING 		10U

#define DPS310_NUM_OF_SCAL_FACTS 		8

// status code
#define DPS310_SUCCEEDED 				0
#define DPS310_FAIL_UNKNOWN 			-1
#define DPS310_FAIL_INIT_FAILED 		-2
#define DPS310_FAIL_TOOBUSY 			-3
#define DPS310_FAIL_UNFINISHED 			-4

enum mode
{
    IDLE = 		0x00,
    CMD_PRS = 	0x01,
    CMD_TEMP =	0x02,
    CMD_BOTH = 	0x03, // only for DPS422
    CONT_PRS = 	0x05,
    CONT_TMP = 	0x06,
    CONT_BOTH = 0x07
};

enum register_blocks_e
{
	PRS = 0, // pressure value
	TEMP,    // temperature value
};

extern reg_block_t registerBlocks[2];

/**
 * @brief registers for configuration and flags; these are the same for both 310 and 422, might need to be adapted for future sensors
 *
 */

extern reg_mask_t config_registers[NUM_OF_COMMON_REGMASKS];
extern reg_mask_t registers[DPS310_NUM_OF_REGMASKS];

enum Config_Registers_e
{
    TEMP_MR = 0, // temperature measure rate
    TEMP_OSR,    // temperature measurement resolution
    PRS_MR,      // pressure measure rate
    PRS_OSR,     // pressure measurement resolution
    MSR_CTRL,    // measurement control
    FIFO_EN,

    TEMP_RDY,
    PRS_RDY,
    INT_FLAG_FIFO,
    INT_FLAG_TEMP,
    INT_FLAG_PRS,
};

enum Registers_e
{
    PROD_ID = 0,
    REV_ID,
    TEMP_SENSOR,    // internal vs external
    TEMP_SENSORREC, //temperature sensor recommendation
    TEMP_SE,        //temperature shift enable (if temp_osr>3)
    PRS_SE,         //pressure shift enable (if prs_osr>3)
    FIFO_FL,        //FIFO flush
    FIFO_EMPTY,     //FIFO empty
    FIFO_FULL,      //FIFO full
    INT_HL,
    INT_SEL,         //interrupt select
};

//typedef enum
//{
//	DPS310_NONE_RDY = 0x00,
//	DPS310_PRS_RDY = 0x10,
//	DPS310_TMP_RDY = 0x20,
//	DPS310_SENSOR_RDY = 0x40,
//	DPS310_COEF_RDY = 0x80
//}DPS310_meas_state_t;

#endif /* DPS310_CONFIG_H_ */
