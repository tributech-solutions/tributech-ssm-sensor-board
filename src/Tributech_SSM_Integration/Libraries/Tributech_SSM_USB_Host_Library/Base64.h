/*
 * Base64.h
 *
 *  Created on: 17 Mar 2020
 *      Author: DanielHackl
 */

#ifndef LIBRARIES_SSMLIB_BASE64_BASE64_H_
#define LIBRARIES_SSMLIB_BASE64_BASE64_H_

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Encode
char* base64_encode( const void* binaryData, uint16_t len, uint16_t *flen );

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Decode
int b64_decode(const char *in, unsigned char *out, size_t outlen);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Decode Size
size_t b64_decoded_size(const char *in);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Generate decode table
void b64_generate_decode_table();

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check if char is valid
int b64_is_valid_char(char c);


#endif /* LIBRARIES_SSMLIB_BASE64_BASE64_H_ */
