/*
 * Tributech_Lib.c
 *
 *  Created on: 17 Aug 2020
 *      Author: DanielHackl
 */

#include "Tributech_Lib.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String To UpperCase
char * to_upper_case(char * text)
{
	uint16_t i;

	i=0;
	while(i < strlen(text))
	{
		text[i] = (char) toupper((int)text[i]);
	    i++;
	}

	return text;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// String To LowerCase
char * to_lower_case(char * text)
{
	uint16_t i;

	i=0;
	while(i < strlen(text))
	{
		text[i] = (char) tolower((int)text[i]);
	    i++;
	}

	return text;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// increase transaction number
void increase_transaction_nr(void)
{
	transaction_nr_dec++;
	if (transaction_nr_dec >= 1000000)
	{
		transaction_nr_dec = 0;
	}
	memset(transaction_nr_string,0x0,7);
	sprintf(transaction_nr_string, "%"PRIu32"",transaction_nr_dec);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Get Time
time_t get_time(void)
{
	time_t Time_Sec;

	RTC_Time(&Time_Sec);

	return Time_Sec;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Get ValueMetaDataId
uint8_t get_value_meta_data_id(char * id_string, uint64_t index)
{
	uint8_t id_hex[17];

	memset(id_hex,0x0,17);

	// Get Device ID  from Chip ID
	get_unique_chip_id(&device_id_hex,device_id_string);

	// Get ValueMetaDataId Format Hex
	get_internal_ValueMetaDataId_hex(&index,id_hex);

	// Transform ValueMetaDataId Hex to String
	transform_ValueMetaDataId_hex_to_string(id_hex,id_string);

	return 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// get unique chip id
uint16_t get_unique_chip_id(uint64_t * hex_value, char * string_value)
{
	uint64_t identification_number = 0;
	uint8_t	shift;
	char tempstring[3];

	//++++++++++++++++++++++++++++++++++++++
	// Read 8 Bytes
	for(uint8_t index = 0; index < 8; index++)
	{
		shift = 8*(7-index);
		identification_number = identification_number + ((uint64_t)g_chipid[7-index] << shift);
		sprintf(tempstring,"%02x",g_chipid[7-index]);
		string_value = strcat(string_value,tempstring);

	}
	*hex_value = identification_number;

	return 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Transform 64 bit value to ValueMetaDataId
uint16_t get_internal_ValueMetaDataId_hex(uint64_t *sensor_id,uint8_t *ValueMetaDataId)
{
	memset(ValueMetaDataId,0x0,16);
	memcpy(ValueMetaDataId,&device_id_hex,8);
	memcpy(ValueMetaDataId+8,sensor_id,8);

	return 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// parse response
uint16_t parse_response(uint16_t response_length)
{
	uint16_t number_of_tokens;		// number of tokens parsed

	t = calloc(64,sizeof(jsmntok_t));
	//+++++++++++++++++++++++++++++++++++++++++++
	// Initial start token
	p.pos = 0;
	p.toknext = 0;
	p.toksuper = -1;

	//+++++++++++++++++++++++++++++++++++++++++++
	// Get tokens by parsing json received text
	number_of_tokens = jsmn_parse(&p, recv_command, response_length, t, 64);//sizeof(t) / sizeof(t[0]));

	return number_of_tokens;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// get hex string from 64 bit value
void get_string_from_64bit(uint8_t *value, char *string)
{
	char tempstring[3];	// temp string variable
	uint8_t temp;		// temp variable
	uint8_t * pt;		// pointer
	int16_t index;		// index

	//++++++++++++++++++++++++++++++++++++++
	// Read 8 Bytes
	for(index = 7; index >= 0; index--)
	{
		pt = value + (uint8_t)index;	// get pointer address
		temp = 0;						// reset temp var
		memcpy(&temp,pt,1);				// copy into temp
		memset(tempstring,0x0,3);		// reset temp string variable

		sprintf(tempstring,"%02x",temp);		// copy it into temp string variable
		string = strcat(string,tempstring);		// extend string
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Transform 64 bit value to ValueMetaDataId string
uint16_t transform_ValueMetaDataId_hex_to_string(uint8_t *value,char *data)
{
	char tempstring[3];

	memset(data,0x0,37);

	sprintf(tempstring,"%02x",value[3]);
	strcat(data,tempstring);
	sprintf(tempstring,"%02x",value[2]);
	strcat(data,tempstring);
	sprintf(tempstring,"%02x",value[1]);
	strcat(data,tempstring);
	sprintf(tempstring,"%02x",value[0]);
	strcat(data,tempstring);

	strcat(data,"-");

	sprintf(tempstring,"%02x",value[5]);
	strcat(data,tempstring);
	sprintf(tempstring,"%02x",value[4]);
	strcat(data,tempstring);

	strcat(data,"-");

	sprintf(tempstring,"%02x",value[7]);
	strcat(data,tempstring);
	sprintf(tempstring,"%02x",value[6]);
	strcat(data,tempstring);

	strcat(data,"-");

	sprintf(tempstring,"%02x",value[8]);
	strcat(data,tempstring);
	sprintf(tempstring,"%02x",value[9]);
	strcat(data,tempstring);

	strcat(data,"-");

	for(uint8_t index = 10; index < 16; index++)
	{
		sprintf(tempstring,"%02x",value[index]);
		strcat(data,tempstring);

	}
	return 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Get description of errorcode
void decode_error_code(int16_t error_code)
{
	switch (error_code)
	{
	case 0:
		strcpy(error_code_string,"success");
		break;
	case -1:
		strcpy(error_code_string,"unknown error code");
		break;
	case -2:
		strcpy(error_code_string,"transaction still in progress");
		break;
	case -3:
		strcpy(error_code_string,"invalid operation");
		break;
	case -4:
		strcpy(error_code_string,"max merkle tree depth exceeded");
		break;
	case -5:
		strcpy(error_code_string,"max size of a single value exceeded");
		break;
	case -6:
		strcpy(error_code_string,"max count of values exceeded");
		break;
	case -7:
		strcpy(error_code_string,"invalid timestamp format");
		break;
	case -8:
		strcpy(error_code_string,"invalid ValueMetaDataId format");
		break;
	case -9:
		strcpy(error_code_string,"invalid endpoints format");
		break;
	case -10:
		strcpy(error_code_string,"parsing error");
		break;
	case -11:
		strcpy(error_code_string,"cycle time too fast -> delay sending");
		break;
	case -12:
			strcpy(error_code_string,"SSM not ready");
			break;
	default:
		strcpy(error_code_string,"unknown error code");
		break;
	}
}
