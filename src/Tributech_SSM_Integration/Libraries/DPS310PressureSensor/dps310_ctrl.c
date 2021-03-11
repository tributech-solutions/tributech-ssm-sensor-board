/*
 * 	dps310.c *
 *  Created on: 2020 Aug 13
 *  Author: AEASCHER
 */

#include "i2c_master_ctrl.h"
#include "stddef.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "dps_register.h"
#include "dps310_config.h"
#include "dps310_ctrl.h"
#include "uart.h"
#include "USB_Glue.h"

const int32_t scaling_facts[DPS310_NUM_OF_SCAL_FACTS] =
{	524288,
	1572864,
	3670016,
	7864320,
	253952,
	516096,
	1040384,
	2088960
};

dps310_state_t dps310_status =
{
	.init_flag 		= false,
	.mode_flag 		= 0,
	.temp_Sensor 	= 0,

	//settings
	//temperature measure rate (value from 0 to 7)
	//2^temp_mr temperature measurement results per second
	.temp_mr = DPS310_OVERSAMPLING_RATE_2,
	//temperature oversampling rate (value from 0 to 7)
	//2^temp_osr internal temperature measurements per result
	//A higher value increases precision
	.temp_osr = DPS310_PRESCISSION_1x,
	//pressure measure rate (value from 0 to 7)
	//2^prs_mr pressure measurement results per second
	.prs_mr = DPS310_OVERSAMPLING_RATE_2,
	//pressure oversampling rate (value from 0 to 7)
	//2^prs_osr internal pressure measurements per result
	//A higher value increases precision
	.prs_osr = DPS310_PRESCISSION_16x,

	//last measurements
	.pres_meas = 0,
	.temp_meas = 0,
};

dps310_comp_coeff_t dps310_coeff =
{
	// compensation coefficients for both dps310 and dps422
	.c0_half = 0,
	.c1 = 0,
	.c00 = 0,
	.c10 = 0,
	.c01 = 0,
	.c11 = 0,
	.c20 = 0,
	.c21 = 0,
	.c30 = 0,


	// last measured scaled temperature (necessary for pressure compensation)
	.last_temp_scal = 0,
};

reg_block_t coeff_block = {0x10, 18};

int16_t dps310_read_byte_bitfield(reg_mask_t reg_mask, bool comparison_timer)
{
	int16_t return_value = i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, reg_mask.register_address, comparison_timer);
	if (return_value < 0)
	{
		return return_value;
	}
	return (((uint8_t)return_value) & reg_mask.mask) >> reg_mask.shift;
}

int16_t dps310_write_byte_bitfield(uint8_t data, reg_mask_t reg_mask, bool comparison_timer)
{
	uint8_t tx_data[2] = {0};
	int16_t error_code = 1;
	int16_t old = i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, reg_mask.register_address, comparison_timer);
	if (old < 0)
	{
		//fail while reading
		return old;
	}
	tx_data[0] = reg_mask.register_address;
	tx_data[1] = ((uint8_t)old & ~reg_mask.mask) | ((data << reg_mask.shift) & reg_mask.mask);
	error_code = i2c_transmit_data(DPS310_ADDRESS_SDO_FLOAT, tx_data, 2, true, true, comparison_timer);
	if(error_code != 0)
	{return -1;}
//	error_code = i2c_transmit_data(DPS310_ADDRESS_SDO_FLOAT, &tx_data[1], 1, false, true);
//	if(error_code != 0)
//		{return -1;}
	return error_code;
}

int16_t dps310_write_byte(uint8_t reg_address, uint8_t data, bool check, bool comparison_timer)
{
	uint8_t tx_data[2] = {0};
	tx_data[0] = reg_address;
	tx_data[1] = data;
	int16_t error_code = i2c_transmit_data(DPS310_ADDRESS_SDO_FLOAT, tx_data, 2, true, true, comparison_timer);
	if (error_code < 0)
	{
		return error_code;
	}
	if (check == false)
	{
		return 0;					  //no checking
	}
	int16_t eval_data = i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, reg_address, comparison_timer);
	if (eval_data == data) //check if desired by calling function
	{

		return DPS310_SUCCEEDED;
	}
	else
	{
		return DPS310_FAIL_UNKNOWN;
	}
}

int16_t dps310_read_register_block(reg_block_t reg_block, uint8_t *buffer, bool comparison_timer)
{
	//do not read if there is no buffer
	if(buffer == NULL)
	{
		return -1;
	}

	int16_t error_code = i2c_read_block(DPS310_ADDRESS_SDO_FLOAT, reg_block.register_address, buffer, reg_block.length, comparison_timer);
	if (error_code < 0)
	{
		return error_code;
	}

	return reg_block.length;
}

void get_twos_complement(int32_t *raw, uint8_t length)
{
	if (*raw & ((uint32_t)1 << (length - 1)))
	{
		*raw -= (uint32_t)1 << length;
	}
}

uint16_t calc_busy_time(uint16_t mr, uint16_t osr)
{
	//formula from datasheet (optimized)
	return ((uint32_t)20U << mr) + ((uint32_t)16U << (osr + mr));
}

float calc_temp(int32_t raw)
{
	float temp = raw;

	//scale temperature according to scaling table and oversampling
	temp /= scaling_facts[dps310_status.temp_osr];

	//update last measured temperature
	//it will be used for pressure compensation
	dps310_coeff.last_temp_scal = temp;

	//Calculate compensated temperature
	temp = dps310_coeff.c0_half + dps310_coeff.c1 * temp;

	return temp;
}

float calc_preassure(int32_t raw)
{
	float prs = raw;

	//scale pressure according to scaling table and oversampling
	prs /= scaling_facts[dps310_status.prs_osr];

	//Calculate compensated pressure
	prs = dps310_coeff.c00 + prs * (dps310_coeff.c10 + prs * (dps310_coeff.c20 + prs * dps310_coeff.c30)) + dps310_coeff.last_temp_scal * (dps310_coeff.c01 + prs * (dps310_coeff.c11 + prs * dps310_coeff.c21));

	//return pressure
	return prs;
}

int16_t dps310_init()
{
	int16_t state = 0;
	//start_up time of DPS310
	XMC_USBH_osDelay(100);

	if(dps310_get_product_id() < 0)
	{
		//connected device is not a Dps310
		dps310_status.init_flag = false;
		return DPS310_FAIL_INIT_FAILED;
	}
	//Softreset to see if everything is in order
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	dps310_write_byte(0x0C, 0b1001, false, false);
	do
	{
		XMC_USBH_osDelay(200);
		state = i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, 0x08, false);
	}while(!(state & (0x80 | 0x40)));

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(dps310_get_revision_id() < 0)
	{
		dps310_status.init_flag = false;
		return DPS310_FAIL_INIT_FAILED;
	}

	//find out which temperature sensor is calibrated with coefficients...
	int16_t sensor = dps310_get_calibrated_temp_sensor();
	if(sensor < 0)
	{
		dps310_status.init_flag = false;
		return DPS310_FAIL_INIT_FAILED;
	}

	// use that sensor for temperature measurement
	if(dps310_write_byte_bitfield((uint8_t)sensor, registers[TEMP_SENSOR], false) < 0)
	{
		dps310_status.init_flag = false;
		return DPS310_FAIL_INIT_FAILED;
	}

	//read coefficients
	if (dps310_get_temp_sensor_coeff() < 0)
	{
		dps310_status.init_flag = false;
		return DPS310_FAIL_INIT_FAILED;
	}

	dps310_status.init_flag = true;
	dps310_set_standby_mode();

	//	// Fix IC with a fuse bit problem, which lead to a wrong temperature
	//	// Should not affect ICs without this problem
	dps310_correct_temperature();

	if(dps310_start_measure_both_cont(dps310_status.temp_mr, dps310_status.temp_osr, dps310_status.prs_mr, dps310_status.prs_osr) < 0)
	{
		dps310_status.init_flag = false;
		return DPS310_FAIL_INIT_FAILED;
	}


	return DPS310_SUCCEEDED;
}

void dps310_deinit(void)
{
	//Softreset to reset the chip
	//A softreset will run through the same sequence as in power-on reset
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	dps310_write_byte(0x0C, 0b1001, false, false);

	// reset init status
	dps310_status.init_flag = 		false;
	dps310_status.mode_flag = 		0;
	dps310_status.temp_Sensor = 	0;
	dps310_status.temp_mr = 		DPS310_OVERSAMPLING_RATE_2;
	dps310_status.temp_osr = 		DPS310_PRESCISSION_1x;
	dps310_status.prs_mr = 			DPS310_OVERSAMPLING_RATE_2;
	dps310_status.prs_osr = 		DPS310_PRESCISSION_16x;
	dps310_status.pres_meas = 		0;
	dps310_status.temp_meas = 		0;
}

int16_t dps310_get_product_id(void)
{
	int16_t product_id = dps310_read_byte_bitfield(registers[PROD_ID], false);
	if (product_id < 0)
	{
		//Connected device is not a Dps310
		return DPS310_FAIL_UNKNOWN;
	}
	return product_id;
}

int16_t dps310_get_revision_id(void)
{
	int16_t revision_id = dps310_read_byte_bitfield(registers[REV_ID], false);
	if (revision_id < 0)
	{
		return DPS310_FAIL_UNKNOWN;
	}
	return revision_id;
}

int16_t dps310_get_calibrated_temp_sensor(void)
{
	int16_t sensor_calib = dps310_read_byte_bitfield(registers[TEMP_SENSORREC], false);
	if (sensor_calib < 0)
	{
		return DPS310_FAIL_UNKNOWN;
	}
	return sensor_calib;
}

int16_t dps310_get_temp_sensor_coeff(void)
{
	uint8_t buffer[18] = {0};

	int16_t error_code = dps310_read_register_block(coeff_block, buffer, false);
	if(error_code < 0)
	{
		return DPS310_FAIL_UNKNOWN;
	}
	//compose coefficients from buffer content
	dps310_coeff.c0_half = ((uint32_t)buffer[0] << 4) | (((uint32_t)buffer[1] >> 4) & 0x0F);
	get_twos_complement(&dps310_coeff.c0_half, 12);
	//c0 is only used as c0*0.5, so c0_half is calculated immediately
	dps310_coeff.c0_half = dps310_coeff.c0_half / 2U;

	//now do the same thing for all other coefficients
	dps310_coeff.c1 = (((uint32_t)buffer[1] & 0x0F) << 8) | (uint32_t)buffer[2];
	get_twos_complement(&dps310_coeff.c1, 12);
	dps310_coeff.c00 = ((uint32_t)buffer[3] << 12) | ((uint32_t)buffer[4] << 4) | (((uint32_t)buffer[5] >> 4) & 0x0F);
	get_twos_complement(&dps310_coeff.c00, 20);
	dps310_coeff.c10 = (((uint32_t)buffer[5] & 0x0F) << 16) | ((uint32_t)buffer[6] << 8) | (uint32_t)buffer[7];
	get_twos_complement(&dps310_coeff.c10, 20);

	dps310_coeff.c01 = ((uint32_t)buffer[8] << 8) | (uint32_t)buffer[9];
	get_twos_complement(&dps310_coeff.c01, 16);

	dps310_coeff.c11 = ((uint32_t)buffer[10] << 8) | (uint32_t)buffer[11];
	get_twos_complement(&dps310_coeff.c11, 16);
	dps310_coeff.c20 = ((uint32_t)buffer[12] << 8) | (uint32_t)buffer[13];
	get_twos_complement(&dps310_coeff.c20, 16);
	dps310_coeff.c21 = ((uint32_t)buffer[14] << 8) | (uint32_t)buffer[15];
	get_twos_complement(&dps310_coeff.c21, 16);
	dps310_coeff.c30 = ((uint32_t)buffer[16] << 8) | (uint32_t)buffer[17];
	get_twos_complement(&dps310_coeff.c30, 16);
	return 0;
}

int16_t dps310_configure_temp_meas(uint8_t temp_mr, uint8_t temp_osr)
{
	temp_mr &= 0x07;
	temp_osr &= 0x07;
	// two accesses to the same register; for readability
	int16_t error_code = dps310_write_byte_bitfield(temp_mr, config_registers[TEMP_MR], false);
	//abort immediately on fail
	if (error_code != DPS310_SUCCEEDED)
	{
		return DPS310_FAIL_UNKNOWN;
	}

	error_code = dps310_write_byte_bitfield(temp_osr, config_registers[TEMP_OSR], false);
	//abort immediately on fail
	if (error_code != DPS310_SUCCEEDED)
	{
		return error_code;
	}
//	error_code = i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, config_registers[TEMP_OSR].register_address);
//	if(error_code != 0)
//	{
//		return error_code;
//	}
	dps310_status.temp_mr = temp_mr;
	dps310_status.temp_osr = temp_osr;

	return DPS310_SUCCEEDED;
}

int16_t dps310_configure_pressure_meas(uint8_t prs_mr, uint8_t prs_osr)
{
	prs_mr &= 0x07;
	prs_osr &= 0x07;
	int16_t error_code = dps310_write_byte_bitfield(prs_mr, config_registers[PRS_MR], false);
	//abort immediately on fail
	if (error_code != DPS310_SUCCEEDED)
	{
		return DPS310_FAIL_UNKNOWN;
	}

	error_code = dps310_write_byte_bitfield(prs_osr, config_registers[PRS_OSR], false);
	//abort immediately on fail
	if (error_code != DPS310_SUCCEEDED)
	{
		return DPS310_FAIL_UNKNOWN;
	}
	dps310_status.prs_mr = prs_mr;
	dps310_status.prs_osr = prs_osr;
	return DPS310_SUCCEEDED;
}

int16_t dps310_start_meas_temp_once(uint8_t oversampling_rate)
{
	//abort if initialization failed
	if (dps310_status.init_flag == false)
	{
		return DPS310_FAIL_INIT_FAILED;
	}
	//abort if device is not in idling mode
	if (dps310_status.mode_flag != IDLE)
	{
		return DPS310_FAIL_TOOBUSY;
	}

	if (oversampling_rate != dps310_status.temp_osr)
	{
		//configuration of oversampling rate
		if (dps310_configure_temp_meas(0U, oversampling_rate) != DPS310_SUCCEEDED)
		{
			return DPS310_FAIL_UNKNOWN;
		}
	}

	//set device to temperature measuring mode
	return dps310_set_operation_mode(CMD_TEMP);
}

int16_t dps310_start_meas_pres_once(uint8_t oversampling_rate)
{
	//abort if initialization failed
	if (dps310_status.init_flag == false)
	{
		return DPS310_FAIL_INIT_FAILED;
	}
	//abort if device is not in idling mode
	if (dps310_status.mode_flag != IDLE)
	{
		return DPS310_FAIL_TOOBUSY;
	}
	//configuration of oversampling rate, lowest measure rate to avoid conflicts
	if (oversampling_rate != dps310_status.prs_osr)
	{
		if (dps310_configure_pressure_meas(0U, oversampling_rate))
		{
			return DPS310_FAIL_UNKNOWN;
		}
	}
	//set device to pressure measuring mode
	return dps310_set_operation_mode(CMD_PRS);
}

int16_t dps310_measure_temp_once(float *result, uint8_t oversampling_rate)
{
	//Start measurement
	int16_t error_code = dps310_start_meas_temp_once(oversampling_rate);
	if (error_code != DPS310_SUCCEEDED)
	{
		return error_code;
	}

	uint32_t busy_time = calc_busy_time(0U, dps310_status.temp_osr) / DPS310_BUSYTIME_SCALING;
	//wait until measurement is finished
	XMC_USBH_osDelay(busy_time);
	XMC_USBH_osDelay(DPS310__BUSYTIME_FAILSAFE);

	error_code = dps310_get_single_result(result);
	if (error_code != DPS310_SUCCEEDED)
	{
		dps310_set_standby_mode();
	}
	return error_code;
}

int16_t dps310_measure_pres_once(float *result, uint8_t oversampling_rate)
{
	//start the measurement
	int16_t ret = dps310_start_meas_pres_once(oversampling_rate);
	if (ret != DPS310_SUCCEEDED)
	{
		return ret;
	}

	uint32_t busy_time = calc_busy_time(0U, dps310_status.temp_osr) / DPS310_BUSYTIME_SCALING;
	//wait until measurement is finished
	XMC_USBH_osDelay(busy_time);
	XMC_USBH_osDelay(DPS310__BUSYTIME_FAILSAFE);

	ret = dps310_get_single_result(result);
	if (ret != DPS310_SUCCEEDED)
	{
		dps310_set_standby_mode();
	}
	return ret;
}

int16_t dps310_disable_FIFO(void)
{
	int16_t error_code = dps310_flush_FIFO();
	error_code = dps310_write_byte_bitfield(0U, config_registers[FIFO_EN], false);
	return error_code;
}

int16_t dps310_enable_FIFO(void)
{
//	int16_t test = 0;
//	test = i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, config_registers[FIFO_EN].register_address);
	return dps310_write_byte_bitfield(1U, config_registers[FIFO_EN], false);
}

int16_t dps310_flush_FIFO(void)
{
	return dps310_write_byte_bitfield(1U, registers[FIFO_FL], true);
}

int16_t dps310_read_FIFO_state(void)
{
	return dps310_read_byte_bitfield(registers[FIFO_FULL], true);
}

int16_t get_FIFO_value(int32_t *value)
{
	uint8_t buffer[DPS310_RESULT_BLOCK_LENGTH] = {0};

	//abort on invalid argument or failed block reading
	if (value == NULL || dps310_read_register_block(registerBlocks[PRS], buffer, false) != DPS310_RESULT_BLOCK_LENGTH)
		return DPS310_FAIL_UNKNOWN;
	*value = (uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[2];
	get_twos_complement(value, 24);
	return buffer[2] & 0x01;
}

int16_t dps310_correct_temperature(void)
{
	if (dps310_status.init_flag == false)
	{
		return DPS310_FAIL_INIT_FAILED;
	}
	dps310_write_byte(0x0E, 0xA5, false, false);
	dps310_write_byte(0x0F, 0x96, false, false);
	dps310_write_byte(0x62, 0x02, false, false);
	dps310_write_byte(0x0E, 0x00, false, false);
	dps310_write_byte(0x0F, 0x00, false, false);

	//perform a first temperature measurement (again)
	//the most recent temperature will be saved internally
	//and used for compensation when calculating pressure
	float trash;
	dps310_measure_temp_once(&trash, dps310_status.temp_osr);

	return DPS310_SUCCEEDED;
}

int16_t dps310_get_single_result(float *result)
{
	//abort if initialization failed
	if (dps310_status.init_flag == false)
	{
		return DPS310_FAIL_INIT_FAILED;
	}

	//read finished bit for current opMode
	int16_t rdy;
	switch (dps310_status.mode_flag)
	{
	case CMD_TEMP: //temperature
		rdy = dps310_read_byte_bitfield(config_registers[TEMP_RDY], true);
		break;
	case CMD_PRS: //pressure
		rdy = dps310_read_byte_bitfield(config_registers[PRS_RDY], true);
		break;
	default: //DPS310 not in command mode
		return DPS310_FAIL_TOOBUSY;
	}

	int8_t oldMode = 0;
	//read new measurement result
	switch (rdy)
	{
	case DPS310_FAIL_UNKNOWN: //could not read ready flag
		return DPS310_FAIL_UNKNOWN;
	case 0: //ready flag not set, measurement still in progress
		return DPS310_FAIL_UNFINISHED;
	case 1: //measurement ready, expected case

		oldMode = dps310_status.mode_flag;
		dps310_status.mode_flag = IDLE; //opcode was automatically reseted by DPS310
		int32_t raw_val;
		switch (oldMode)
		{
		case CMD_TEMP: //temperature
			dps310_get_raw_result(&raw_val, registerBlocks[TEMP]);
			*result = calc_temp(raw_val);
			return DPS310_SUCCEEDED; // TODO
		case CMD_PRS:			   //pressure
			dps310_get_raw_result(&raw_val, registerBlocks[PRS]);
			*result = calc_preassure(raw_val);
			return DPS310_SUCCEEDED; // TODO
		default:
			return DPS310_FAIL_UNKNOWN; //should already be filtered above
		}
	}
	return DPS310_FAIL_UNKNOWN;
}

int16_t dps310_get_raw_result(int32_t *raw, reg_block_t reg)
{
	uint8_t buffer[DPS310_RESULT_BLOCK_LENGTH] = {0};
	if (dps310_read_register_block(reg, buffer, true) != DPS310_RESULT_BLOCK_LENGTH)
		return DPS310_FAIL_UNKNOWN;

	*raw = (uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[2];
	get_twos_complement(raw, 24);
	return DPS310_SUCCEEDED;
}

int16_t dps310_start_measure_both_cont(uint8_t temp_mr,
									   uint8_t temp_osr,
									   uint8_t prs_mr,
									   uint8_t prs_osr)
{
	//abort if initialization failed
	if (dps310_status.init_flag == false)
	{
		return DPS310_FAIL_INIT_FAILED;
	}
	//abort if device is not in idling mode
	if (dps310_status.mode_flag != IDLE)
	{
		return DPS310_FAIL_TOOBUSY;
	}
	//abort if speed and precision are too high
	if (calc_busy_time(temp_mr, temp_osr) + calc_busy_time(prs_mr, prs_osr) >= DPS310__MAX_BUSYTIME)
	{
		return DPS310_FAIL_UNFINISHED;
	}
//	//clean temp_config reg and pressure_config reg
//	dps310_write_byte(config_registers[TEMP_MR].register_address, 0b10000000, true);
//	//clean config register
//	dps310_write_byte(0x09, 0x03, true);
	//update precision and measuring rate
	if (dps310_configure_temp_meas(temp_mr, temp_osr))
	{
		return DPS310_FAIL_UNKNOWN;
	}
	if(temp_osr > DPS310_OVERSAMPLING_RATE_8)
	{
		reg_mask_t reg_mask = {0x09, 0x08, 3};
		dps310_write_byte_bitfield(1U, reg_mask, false);
	}
	else
	{
		reg_mask_t reg_mask = {0x09, 0x08, 3};
		dps310_write_byte_bitfield(0U, reg_mask, false);
	}

	//update precision and measuring rate
	if (dps310_configure_pressure_meas(prs_mr, prs_osr))
	{
		return DPS310_FAIL_UNKNOWN;
	}
	if(prs_osr > DPS310_OVERSAMPLING_RATE_8)
	{
		reg_mask_t reg_mask = {0x09, 0x04, 2};
		dps310_write_byte_bitfield(1U, reg_mask, false);
	}
	else
	{
		reg_mask_t reg_mask = {0x09, 0x04, 2};
		dps310_write_byte_bitfield(0U, reg_mask, false);
	}
//	//enable result FIFO
//	if (dps310_enable_FIFO())
//	{
//		return DPS310_FAIL_UNKNOWN;
//	}
	//Start measuring in background mode
	if (dps310_set_operation_mode(CONT_BOTH))
	{
		return DPS310_FAIL_UNKNOWN;
	}
	return DPS310_SUCCEEDED;
}

int16_t dps310_get_cont_results()
{
	char write_buffer[50] = {0};
	uint8_t buffer[3] = {0};
	int32_t value = 0;
	if(dps310_status.init_flag != true)
	{
		return DPS310_FAIL_INIT_FAILED;
	}
	//abort if device is not in background mode
	if (!(dps310_status.mode_flag & 0x04))
	{
		return DPS310_FAIL_TOOBUSY;
	}
	if(dps310_read_FIFO_state() == 1)
	{
		dps310_flush_FIFO();
	}
	int16_t meas_state = (i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, config_registers[MSR_CTRL].register_address, true));// & 0xF0);
	if(meas_state & 0x20)
	{
		i2c_read_block(DPS310_ADDRESS_SDO_FLOAT, registerBlocks[TEMP].register_address, buffer, registerBlocks[TEMP].length, true);
		value = (uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[2];
		get_twos_complement(&value, 24);
		dps310_status.temp_meas = calc_temp(value);
		sprintf(&write_buffer[0], "Temperature: %f \r\n", dps310_status.temp_meas);
		UART_Transmit(&write_buffer[0], 50);
	}
	if (meas_state & 0x10)
	{
		i2c_read_block(DPS310_ADDRESS_SDO_FLOAT, registerBlocks[PRS].register_address, buffer, registerBlocks[PRS].length, true);
		value = (uint32_t)buffer[0] << 16 | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[2];
		get_twos_complement(&value, 24);
		dps310_status.pres_meas = calc_preassure(value);
		sprintf(&write_buffer[0], "Pressure: %f \r\n", dps310_status.pres_meas);
		UART_Transmit(&write_buffer[0], 50);
	}
	return DPS310_SUCCEEDED;
}

int16_t dps310_set_operation_mode(uint8_t op_mode)
{
	if (dps310_write_byte_bitfield(op_mode, config_registers[MSR_CTRL], false) == -1)
	{
		return DPS310_FAIL_UNKNOWN;
	}
	dps310_status.mode_flag = op_mode;
	return DPS310_SUCCEEDED;
}

int16_t dps310_set_standby_mode(void)
{
	//failes if init flag is false
	if(dps310_status.init_flag == false)
	{
		return DPS310_FAIL_UNKNOWN;
	}
	//set device to idling mode
	int16_t error_code = dps310_set_operation_mode(IDLE);
	if (error_code != DPS310_SUCCEEDED)
	{
		return error_code;
	}
	// check if standby mode is achieved for testing
//	int16_t check_meas_ctrl = i2c_read_byte(DPS310_ADDRESS_SDO_FLOAT, config_registers[MSR_CTRL].register_address);
//	error_code = dps310_disable_FIFO();
	return error_code;
}



