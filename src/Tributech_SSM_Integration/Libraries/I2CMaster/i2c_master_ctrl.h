/*
 * 	i2C_master_config.h *
 *  Created on: 2020 Aug 13
 *  Author: AEASCHER
 */

#ifndef I2C_MASTER_CONFIG_H_
#define I2C_MASTER_CONFIG_H_

#include "stdint.h"
#include "stdbool.h"

uint8_t i2c_init();
int16_t i2c_transmit_data(uint8_t device_address, uint8_t *data, uint8_t data_length, bool send_start, bool send_stop, bool comparisontimer);
int16_t i2c_read_byte(uint8_t device_address, uint8_t register_address, bool comparisontimer);
int16_t i2c_read_block(uint8_t device_address, uint8_t register_address, uint8_t *buffer,uint8_t length, bool comparisontimer);

#endif /*I2C_MASTER_CONFIG_H_*/
