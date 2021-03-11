/*
 * USB_Host.h
 *
 *  Created on: 14 Aug 2020
 *      Author: DanielHackl
 */

#ifndef LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_USB_HOST_H_
#define LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_USB_HOST_H_

#include <stdint.h>
#include <stdio.h>
#include "xmc_usbh.h"
#include "USB/GLUE/USB_Glue.h"
#include "USBH_VCOM/USBVCH.h"
#include "uart.h"
#include "jsmn.h"

uint8_t USBHostState;
int8_t ReceivedByte;
uint8_t *RxPtr;
uint32_t Rxlen;
uint32_t uart_data_count;
char * send_buffer;
char * recv_command;
uint16_t recv_command_length;
uint8_t request_send;

extern bool BULK_OUT_TransferCompleted;
extern bool NOTIFICATION_TransferCompleted;
extern uint8_t SerialState[SERIAL_STATE_SIZE];
extern uint8_t UART_RX_Buffer[UART_RX_BUFF_SIZE];
/*USB receive buffer*/
#define USB_RX_BUFFER_SIZE  1024
uint8_t RxBuffer[USB_RX_BUFFER_SIZE];

void usb_host_init(void);
void usb_host_communication(void);

/*Callback function executed on port interrupt*/
void CDC_USB_PortCb(uint8_t port, uint32_t event);

/*Function handles BULK OUT and NOTIFICATION pipe transfer completion*/
void CDC_TransferCompletion_Handle(void);

/*Clock configuration*/
void ClockSetup(void);
int16_t receive_response(void);
uint16_t get_id (uint64_t * id);

#endif /* LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_USB_HOST_H_ */
