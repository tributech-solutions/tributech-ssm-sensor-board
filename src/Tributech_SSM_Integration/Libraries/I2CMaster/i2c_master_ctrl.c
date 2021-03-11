/*
 * 	i2C_master_config.c *
 *  Created on: 2020 Aug 13
 *  Author: AEASCHER
 */
#include "i2c_master_ctrl.h"
#include "i2c_master.h"
#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"
#include "Tributech_Lib.h"

 volatile bool tx_finished_0 = true;
 volatile bool rx_finished_0 = true;
 volatile bool i2c_initialized = false;

 uint8_t i2c_init()
 {
	 uint8_t return_code = I2C_MASTER_STATUS_FAILURE;
	 if(i2c_initialized == false)
	 {
		 return_code = I2C_MASTER_Init(&I2C_MASTER_0);
		 if(return_code == 0)
		 {
			 i2c_initialized = true;
		 }
	 }
	 return return_code;
 }

int16_t i2c_transmit_data(uint8_t device_address, uint8_t *data, uint8_t data_length, bool send_start, bool send_stop, bool comparisontimer)
{
	time_t time_recorder_before_sending = 0;
	time_t time_recorder = 0;
	if(data == NULL || device_address == 0)
	{
		return -1;
	}
	uint8_t error_code = -1;
	//Transmit which data
	error_code = I2C_MASTER_Transmit(&I2C_MASTER_0, send_start, (uint8_t)(device_address<<1), data,(uint64_t) data_length, send_stop);
	if(error_code != I2C_MASTER_STATUS_SUCCESS)
	{
		return -1;
	}
	// Wait until tx operation is finished
	if(comparisontimer == false)
	{
		while(I2C_MASTER_IsTxBusy(&I2C_MASTER_0));
	}
	else
	{
		time_recorder_before_sending = get_time();
		while(I2C_MASTER_IsTxBusy(&I2C_MASTER_0) || (time_recorder - time_recorder_before_sending) < 1)
		{
			time_recorder = get_time();
		};
	}

	return 0;
}

int16_t i2c_read_byte(uint8_t device_address, uint8_t register_address, bool comparisontimer)
{
	time_t time_recorder_before_sending = 0;
	time_t time_recorder = 0;
	if(device_address == 0 || register_address == 0)
	{
		return -1;
	}
	uint8_t rx_buffer = 0;
	uint8_t error_code = 1;

	//Transmit which data register you want to read
	error_code = I2C_MASTER_Transmit(&I2C_MASTER_0, true, (uint8_t)(device_address<<1), &register_address, 1, false);
	if(error_code != I2C_MASTER_STATUS_SUCCESS)
	{
		return -1;
	}
	// Wait until tx operation is finished
	if(comparisontimer == false)
	{
		while(I2C_MASTER_IsTxBusy(&I2C_MASTER_0));
	}
	else
	{
		time_recorder_before_sending = get_time();
		while(I2C_MASTER_IsTxBusy(&I2C_MASTER_0) || (time_recorder - time_recorder_before_sending) < 1)
		{
			time_recorder = get_time();
		};
	}
	//receive register byte
	error_code = I2C_MASTER_Receive(&I2C_MASTER_0, true, (uint8_t)(device_address<<1), &rx_buffer, 1, true, true);
	if(error_code != I2C_MASTER_STATUS_SUCCESS)
	{
		return -1;
	}
	//wait until rx operation is done true
	if(comparisontimer == false)
	{
		while(I2C_MASTER_IsRxBusy(&I2C_MASTER_0));
	}
	else
	{
		time_recorder_before_sending = get_time();
		while(I2C_MASTER_IsRxBusy(&I2C_MASTER_0) || (time_recorder - time_recorder_before_sending) < 1)
		{
			time_recorder = get_time();
		};
	}
	return rx_buffer;
}

int16_t i2c_read_block(uint8_t device_address, uint8_t register_address, uint8_t *buffer,uint8_t length, bool comparisontimer)
{
	time_t time_recorder_before_sending = 0;
	time_t time_recorder = 0;
	if(device_address == 0)
	{
		return -1;
	}
	uint8_t error_code = -1;
	//Transmit which data register you want to read
	error_code = I2C_MASTER_Transmit(&I2C_MASTER_0, true, (uint8_t)(device_address<<1), &register_address, 1, false);
	if(error_code != I2C_MASTER_STATUS_SUCCESS)
	{
		return -1;
	}
	// Wait until tx operation is finished
	if(comparisontimer == false)
	{
		while(I2C_MASTER_IsTxBusy(&I2C_MASTER_0));
	}
	else
	{
		time_recorder_before_sending = get_time();
		while(I2C_MASTER_IsTxBusy(&I2C_MASTER_0) || (time_recorder - time_recorder_before_sending) < 1)
		{
			time_recorder = get_time();
		};
	}

	//receive register byte
	error_code = I2C_MASTER_Receive(&I2C_MASTER_0, true, (uint8_t)(device_address<<1), buffer, length, true, true);
	if(error_code != I2C_MASTER_STATUS_SUCCESS)
	{
		return -1;
	}
	//wait until rx operation is done true
	if(comparisontimer == false)
	{
		while(I2C_MASTER_IsRxBusy(&I2C_MASTER_0));
	}
	else
	{
		time_recorder_before_sending = get_time();
		while(I2C_MASTER_IsRxBusy(&I2C_MASTER_0) || (time_recorder - time_recorder_before_sending) < 1)
		{
			time_recorder = get_time();
		};
	}

	return 0;
}
