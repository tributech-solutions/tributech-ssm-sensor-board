/*
 * Tributech_SSM_API.h
 *
 *  Created on: 17 Aug 2020
 *      Author: DanielHackl
 */

#ifndef LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_TRIBUTECH_SSM_API_H_
#define LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_TRIBUTECH_SSM_API_H_

#include <stdint.h>
#include <stdio.h>
#include "jsmn.h"
#include "USB_Host.h"
#include "Tributech_Lib.h"
#include "Base64.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// provide values
uint16_t provide_values(char * id, uint64_t timestamp, uint8_t * value, uint16_t value_size);


#endif /* LIBRARIES_TRIBUTECH_SSM_USB_HOST_LIBRARY_TRIBUTECH_SSM_API_H_ */
