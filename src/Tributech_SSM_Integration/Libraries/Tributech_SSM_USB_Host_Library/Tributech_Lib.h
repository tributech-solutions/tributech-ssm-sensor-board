/*
 * Tributech_Lib.h
 *
 *  Created on: 17 Aug 2020
 *      Author: DanielHackl
 */

#ifndef LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_TRIBUTECH_LIB_H_
#define LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_TRIBUTECH_LIB_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "jsmn.h"
#include "USB_Host.h"
#include <inttypes.h>
#include <rtc.h>

jsmn_parser p;		// json parser
jsmntok_t *t; 		// tokens -> we expect not more than 64

uint64_t device_id_hex;			// device ID hex format
char device_id_string[20];		// device ID string format

uint32_t transaction_nr_dec;	// transaction number decimal
char transaction_nr_string[7];	// transaction number string

char error_code_string[50];		// error code string

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String To LowerCase and UpperCase
char * to_lower_case(char * text);
char * to_upper_case(char * text);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse response
uint16_t parse_response(uint16_t response_length);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Get Time
time_t get_time(void);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// get hex string from 64 bit value
void get_string_from_64bit(uint8_t *value, char *string);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// get unique chip id
uint16_t get_unique_chip_id(uint64_t * hex_value, char * string_value);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Transform 64 bit value to ValueMetaDataId string
uint16_t transform_ValueMetaDataId_hex_to_string(uint8_t *value,char *data);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Transform 64 bit value to ValueMetaDataId
uint16_t get_internal_ValueMetaDataId_hex(uint64_t *sensor_id,uint8_t *ValueMetaDataId);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Get ValueMetaDataId
uint8_t get_value_meta_data_id(char * id_string, uint64_t index);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Get description of errorcode
void decode_error_code(int16_t error_code);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// increase transaction number
void increase_transaction_nr(void);

#endif /* LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_TRIBUTECH_LIB_H_ */
