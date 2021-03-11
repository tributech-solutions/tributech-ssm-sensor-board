/*
 * USB_Host.c
 *
 *  Created on: 14 Aug 2020
 *      Author: DanielHackl
 */

#include <xmc_common.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

#include "USB_Host.h"
#include "Tributech_Lib.h"
#include <xmc_uart.h>

uint32_t Counter;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Data structure for initializing the clock functional block
const XMC_SCU_CLOCK_CONFIG_t CLOCK_XMC4_0_CONFIG =
{
  .syspll_config.n_div = 80U,
  .syspll_config.p_div = 2U,

  .syspll_config.k_div = 4U,

  .syspll_config.mode = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
  .syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
  .enable_oschp = true,

  .enable_osculp = false,

  .calibration_mode = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
  .fstdby_clksrc = XMC_SCU_HIB_STDBYCLKSRC_OSI,
  .fsys_clksrc = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
  .fsys_clkdiv = 1U,
  .fcpu_clkdiv = 1U,
  .fperipheral_clkdiv = 1U
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Callback functions to be called from USB glue layer
USBH_GLUE_APP_IF_t USBVCH_CB =
{
    .GetBytesInPipe = USBVCH_Pipe_BytesInPipe,
    .GetReadByte = USBVCH_Pipe_Read_8,
    .IsINReceived = USBVCH_Pipe_IsINReceived,
    .PipeEventHandler = USBH_VCOM_Rx_Data_Handler,
    .PortEventHandler = CDC_USB_PortCb
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Flag to identify remote wakeup event
volatile uint32_t USBH_RemoteWkUp_Detected = 0U;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Init USB Host Interface
void usb_host_init(void)
{
	send_buffer = calloc(500,sizeof(char));
	recv_command = calloc(USB_RX_BUFFER_SIZE,sizeof(char));


	/*Select VBUS pin as P3.2*/
//	XMC_USBH_Select_VBUS(XMC_GPIO_PORT3, 2U);

	/* Initializes the USB host driver. */
	USB_Init_Host(&USBVCH_CB);

	/*LED pin to indicate USB status*/
	XMC_GPIO_SetMode(XMC_GPIO_PORT5, 9, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT5, 9);
}



int16_t receive_response(void)
{
	uint8_t successfully_received;

	successfully_received = 0;
	Rxlen = 0;
	RxPtr = RxBuffer;
	do
	{
		ReceivedByte = -1;
		USBVCH_ReceiveByte(&ReceivedByte);
		if(ReceivedByte != -1 && !((ReceivedByte == 0xA || ReceivedByte == 0xD) && Rxlen == 0))
		{
		  *RxPtr = ReceivedByte;
		  Rxlen++;
		  RxPtr++;
		  if(Rxlen == USB_RX_BUFFER_SIZE)
		  {
			break;
		  }
		}

		if(ReceivedByte == 0xA && Rxlen > 0)
		{
			uint8_t result = strcmp(send_buffer,(char*)RxBuffer);
			if (result == 0)
			{
				memset(RxBuffer,0x0,USB_RX_BUFFER_SIZE);
				Rxlen = 0;
				RxPtr = RxBuffer;
			}
			else
			{
				successfully_received = 1;
			}
			//break;
		}
	} while(successfully_received == 0 /*ReceivedByte != -1*/);

	/*Transmit the received data to UART channel*/
	if (Rxlen > 0)
	{
	  recv_command_length = Rxlen;
	  memset(recv_command,0x0,USB_RX_BUFFER_SIZE);
	  memcpy(recv_command,RxBuffer,Rxlen);
	  UART_Transmit(recv_command, recv_command_length-2);

	  return 0;
	}
	return 1;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// USB Host Communication
void usb_host_communication(void)
{
	/*Check if remote wakeup is detected and clear the resume
	 * bit after 20ms delay*/
	if (USBH_RemoteWkUp_Detected)
	{
	  (void)XMC_USBH_osDelay(20U);
	  XMC_USBH_TurnOffResumeBit();
	  USBH_RemoteWkUp_Detected = 0;
	}

	USB_GetHostState(&USBHostState);

	/* Check whether device is in configured status */
	if(USBHostState == HOST_STATE_Configured)
	{
	  XMC_GPIO_SetOutputLow(XMC_GPIO_PORT5, 9);
	}

	USBVCH_Process();
	/* Keep calling this function for USB management */
	USB_USBTask();
	/*Handle transfer completion on BULK OUT and NOTIFICATION*/
	CDC_TransferCompletion_Handle();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Callback function executed on port interrupt
void CDC_USB_PortCb(uint8_t port, uint32_t event)
{
  if(event & XMC_USBH_EVENT_DISCONNECT)
  {
    XMC_GPIO_SetOutputHigh(XMC_GPIO_PORT5, 9);
    Driver_USBH0.PipeDelete(USBHost_Pipe_State[0].pipe_handle);
    Driver_USBH0.PipeDelete(USBHost_Pipe_State[1].pipe_handle);
    Driver_USBH0.PipeDelete(USBHost_Pipe_State[2].pipe_handle);
    Driver_USBH0.PipeDelete(USBHost_Pipe_State[3].pipe_handle);
	  /*Reset the data handling indices*/
    USBH_VCOM_RX_cur_index = 0;
    USBH_VCOM_RX_prev_index = 0;
  }
  if(event & XMC_USBH_EVENT_REMOTE_WAKEUP)
  {
    /*This flag is set to remember the occurrence of remote wakeup event and
     * to return from ISR immediately. This helps to time a 20ms delay in
     * the context of the application main loop rather than inside the ISR context.
     * The reason for this is that the example uses a timer interrupt whose priority is
     * lower than the USB interrupt and therefore would result in a deadlock.*/
    USBH_RemoteWkUp_Detected = 1;
  }
}

/*Function handles BULK OUT and NOTIFICATION pipe transfer completion*/
void CDC_TransferCompletion_Handle(void)
{
  if(NOTIFICATION_TransferCompleted)
  {
    /*Notification received from device.
     * Data is available in the buffer SerialState[]*/
  }

  if(BULK_OUT_TransferCompleted)
  {
    /*Application code to be executed when BULK OUT transfer completes*/
  }
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*Clock configuration*/
void ClockSetup(void)
{
  /* Initialize the SCU clock */
  XMC_SCU_CLOCK_Init(&CLOCK_XMC4_0_CONFIG);
  /* RTC source clock */
  XMC_SCU_HIB_SetRtcClockSource(XMC_SCU_HIB_RTCCLKSRC_OSI);

  /*CLOCK_XMC4_USBCLK_ENABLED*/
  /* USB/SDMMC source clock */
  XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
  /* USB/SDMMC divider setting */
  XMC_SCU_CLOCK_SetUsbClockDivider(4U);
  /* Start USB PLL */
  XMC_SCU_CLOCK_StartUsbPll(1U, 32U);
}







