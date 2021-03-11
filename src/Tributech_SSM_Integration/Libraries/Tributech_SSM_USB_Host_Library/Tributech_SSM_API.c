/*
 * Tributech_SSM_API.c


 *
 *  Created on: 17 Aug 2020
 *      Author: DanielHackl
 */

#include "Tributech_SSM_API.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// get id
uint16_t get_id (uint64_t * id)
{
	int16_t error_code;
	//uint64_t id_temp;
	char * key_name;							// object key name
	uint8_t length;								// length of key name
	uint16_t number_of_tokens;


	memset(send_buffer,0x0,500);
	strcpy(send_buffer,"{\"TransactionNr\": 10000, \"Operation\": \"GetId\"}");
	strcat(send_buffer,"\r\n");

	length = strlen(send_buffer);
	USBVCH_SendData((uint8_t*) send_buffer, length, NULL);		// write new line characters

	memset(recv_command,0x0,USB_RX_BUFFER_SIZE);
	error_code = receive_response();
	if (error_code == 0)
	{
		number_of_tokens = parse_response(strlen(recv_command));

		for(uint8_t j = 0; j < number_of_tokens; j++)		// begin with 5
		{

			if (t[j].type == JSMN_STRING)			// search for key_name
			{
				length = t[j].end-t[j].start;
				key_name = calloc(length+1, sizeof(char));
				memcpy(key_name,&recv_command[t[j].start],length);	// get object key name

				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				// assess if error code is zero
				if (strcmp(to_lower_case(key_name),"errorcode") == 0 )
				{
					j++;
					if ((t[j].type == JSMN_PRIMITIVE))
					{
						error_code = strtol(&recv_command[t[j].start], NULL, 0);
					}
				}
				free(key_name);
			}
		}

		free(t);
		return error_code;

	}

	return error_code;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// provide values
uint16_t provide_values (char * id, uint64_t timestamp, uint8_t * value, uint16_t value_size)
{
	int16_t error_code;				// error code
	char * key_name;				// object key name
	uint8_t length;					// length of key name
	char * timestamp_string;		// timestamp string
	char * base64_string;			// Base64 string
	uint16_t base64_length;			// Base64 length
	uint16_t send_offset;			// send offset
	uint8_t * send_buffer_ptr;		// send buffer address
	uint16_t number_of_tokens;		// number of json tokens from received string

	increase_transaction_nr();		// Get transaction number

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// get timestamp
	timestamp_string = calloc(50,sizeof(char));
	get_string_from_64bit((uint8_t*)&timestamp, timestamp_string);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// encode value in base64
	base64_string = base64_encode(value,value_size,&base64_length);

	memset(send_buffer,0x0,500);
	strcpy(send_buffer,"{\"TransactionNr\": ");
	strcat(send_buffer,transaction_nr_string);
	strcat(send_buffer,", \"Operation\": \"ProvideValues\", \"ValueMetaDataId\": \"");
	strcat(send_buffer,id);
	strcat(send_buffer,"\",\"Values\":[{\"Timestamp\": ");
	strcat(send_buffer,timestamp_string);
	strcat(send_buffer,", \"Value\": \"");
	strcat(send_buffer,base64_string);
	strcat(send_buffer,"\"}]}\r\n");

	UART_Transmit(send_buffer, strlen(send_buffer));

	free(timestamp_string);
	free(base64_string);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// send command
	length = strlen(send_buffer);
	send_offset = 0;
	send_buffer_ptr = (uint8_t*)send_buffer;
	do
	{
		USBVCH_SendData((uint8_t*) send_buffer_ptr, length, &send_offset);		// each send only 64 Bytes, cause of pipe size
		length = length - send_offset;	// send_offset returns the sum of bytes which where send
		send_buffer_ptr = send_buffer_ptr + send_offset;
		if (send_offset == 0)
		{
			return 0;
		}
	}while(length > 0);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// receive
	memset(recv_command,0x0,USB_RX_BUFFER_SIZE);
	error_code = receive_response();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// parse error code from json
	if (error_code == 0)
	{
		number_of_tokens = parse_response(strlen(recv_command));

		for(uint8_t j = 0; j < number_of_tokens; j++)		// begin with 5
		{

			if (t[j].type == JSMN_STRING)			// search for key_name
			{
				length = t[j].end-t[j].start;
				key_name = calloc(length+1, sizeof(char));
				memcpy(key_name,&recv_command[t[j].start],length);	// get object key name

				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				// Merkle Tree properties
				if (strcmp(to_lower_case(key_name),"errorcode") == 0 )
				{
					j++;
					if ((t[j].type == JSMN_PRIMITIVE))
					{
						error_code = strtol(&recv_command[t[j].start], NULL, 0);
						decode_error_code(error_code);
						UART_Transmit(" -> ",4);
						UART_Transmit(error_code_string, strlen(error_code_string));
						UART_Transmit("\r\n",2);
					}
				}
				free(key_name);
			}
		}

		free(t);
		return error_code;

	}

	return error_code;
}
