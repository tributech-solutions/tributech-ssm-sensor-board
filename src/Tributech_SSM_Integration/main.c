/******************************************************************************
 * Software License Agreement
 *
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share
 * modifications, enhancements or bug fixes with Infineon Technologies AG
 * (dave@infineon.com).
 *
 *****************************************************************************/
/**
 * @file main.c
 * @date 30 July, 2016
 * @version 1.0.0
 *
 * @brief Implements an example of USB host CDC ACM class <br>
 *
 * <b>Detailed description of file</b> <br>
 * The file implements the main function where it initializes USB peripheral, UART peripheral,
 * LUFA stack and executes the device enumeration and character echo application.
 *
 * History
 *
 * 01 Jul 2014 Version 1.0.0 <br>
 * Initial version
 */

#include "USB_Host.h"
#include "stdio.h"
#include "RTC.h"
#include "uart.h"
#include "Tributech_SSM_API.h"
#include "i2c_master_ctrl.h"
#include "dps310_ctrl.h"
//#include "watchdog.h"

#define true 1
#define false 0
float last_temp_reading = 0;
float last_pres_reading = 0;

uint16_t return_code = 0;

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the place-holder for user application
 * code.
 */

int main(void)
{

	uint8_t request_provide_values;			// Request provide values

	uint8_t cycle_time_s;					// cycle time of sending
	time_t last_publish;					// timestamp of last publish
	time_t last_command_sent;				// timestamp of last sent command via USB

	// input ValueMetaDataId
	char ValueMetaDataId_1[37] = "050480c1-8008-7108-ffff-000000000001";	// ValueMetaDataId 1
	char ValueMetaDataId_2[37] = "050480c1-8008-7108-ffff-000000000002";	// ValueMetaDataId 2

	uint8_t send_temperature;				// send temperature

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Init Clock and RTC (is needed for get_time()-function
	ClockSetup();

	RTC_Init(&RTC_0);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//Init Watchdog
//	WATCHDOG_Init(&WATCHDOG_0);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Init UART
	UART_init();

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Init USB Host Interface
	usb_host_init();

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Init i2c Interface
	i2c_init();


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//init dps310
	dps310_init();
	send_temperature = true;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Set requests
	request_provide_values = true;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Set cycle time - minimum 1s
	cycle_time_s = 1;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Init timestamps
	last_publish = get_time();
	last_command_sent = get_time();

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Init Rest
	transaction_nr_dec = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Start the watchdog timer
//	WATCHDOG_Start();

	UART_Transmit("Hello\r\n",7);
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// cycle code
	while(1U)
	{
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Reset Watchdog Counter
//		WATCHDOG_Service();

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// USB Communication
		usb_host_communication();

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// request dsp310 sensor values
		if(USBHostState == HOST_STATE_Configured  && last_command_sent < get_time())
		{
			//++++++++++++++++++++++++++++++++++++++++++++++++++++
			//get both temperature and pressure
			//measurements can be taken form dps310_status.temp_meas and dps310_status.pres_meas
			//both of these values are float values
			dps310_get_cont_results();
		}

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Send Values via provide values
		if (get_time() >= last_publish + cycle_time_s)
		{
			request_provide_values = true;
		}

		if (request_provide_values == true && USBHostState == HOST_STATE_Configured && last_command_sent < get_time())
		{

			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// Send temperature
			if (send_temperature == true)
			{
				if(last_temp_reading != dps310_status.temp_meas)
				{
					UART_Transmit("Provide Values\r\n",16);
					return_code = provide_values(ValueMetaDataId_1, 0, (uint8_t*) &dps310_status.temp_meas, sizeof(dps310_status.temp_meas));
					send_temperature = false;
					last_temp_reading = dps310_status.temp_mr;
				}
			}
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// Send pressure
			else
			{
				if(last_pres_reading != dps310_status.pres_meas)
				{
					UART_Transmit("Provide Values\r\n",16);
					return_code = provide_values(ValueMetaDataId_2, 0, (uint8_t*) &dps310_status.pres_meas, sizeof(dps310_status.pres_meas));
					send_temperature = true;
					last_pres_reading = dps310_status.prs_mr;
				}
			}


			request_provide_values = false;
			last_publish = get_time();
			last_command_sent = get_time();
		}
	}
}


