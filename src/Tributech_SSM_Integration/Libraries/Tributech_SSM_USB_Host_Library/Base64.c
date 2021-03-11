/*
 * Base64.c
 *
 *  Created on: 17 Mar 2020
 *      Author: DanielHackl
 */
#include <stdlib.h>
#include "stdint.h"
#include "base64.h"
#include <string.h>


const static char b64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;
const static int b64invs[] = { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
	59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50, 51 };


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Base64 Encode
char* base64_encode( const void* binaryData, uint16_t len, uint16_t *flen )
{
  const unsigned char* bin = (const unsigned char*) binaryData ;
  char * result;

  int rc = 0 ; // result counter
  int byteNo ; // I need this after the loop

  int modulusLen = len % 3 ;
  int pad = ((modulusLen&1)<<1) + ((modulusLen&2)>>1) ; // 2 gives 1 and 1 gives 2, but 0 gives 0.

  *flen = 4*(len + pad)/3 ;
  result = (char*) calloc( *flen + 1,sizeof(char) ) ; // and one for the null
  if( !result )
  {
//    puts( "ERROR: base64 could not allocate enough memory." ) ;
//    puts( "I must stop because I could not get enough" ) ;
    return NULL;
  }

  for( byteNo = 0 ; byteNo <= len-3 ; byteNo+=3 )
  {
    unsigned char BYTE0=bin[byteNo];
    unsigned char BYTE1=bin[byteNo+1];
    unsigned char BYTE2=bin[byteNo+2];
    result[rc++]  = b64[ BYTE0 >> 2 ] ;
    result[rc++]  = b64[ ((0x3&BYTE0)<<4) + (BYTE1 >> 4) ] ;
    result[rc++]  = b64[ ((0x0f&BYTE1)<<2) + (BYTE2>>6) ] ;
    result[rc++]  = b64[ 0x3f&BYTE2 ] ;
  }

  if( pad==2 )
  {
	result[rc++] = b64[ bin[byteNo] >> 2 ] ;
	result[rc++] = b64[ (0x3&bin[byteNo])<<4 ] ;
	result[rc++] = '=';
	result[rc++] = '=';
  }
  else if( pad==1 )
  {
	result[rc++]  = b64[ bin[byteNo] >> 2 ] ;
	result[rc++]  = b64[ ((0x3&bin[byteNo])<<4)   +   (bin[byteNo+1] >> 4) ] ;
	result[rc++]  = b64[ (0x0f&bin[byteNo+1])<<2 ] ;
	result[rc++] = '=';
  }

  result[rc]=0; // NULL TERMINATOR! ;)
  return result;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Decode Size
size_t b64_decoded_size(const char *in)
{
	size_t len;
	size_t ret;
	size_t i;

	if (in == NULL)
		return 0;

	len = strlen(in);

	ret = len / 4 * 3;

	for (i=len; i-->0; ) {
		if (in[i] == '=') {
			ret--;
		} else {
			break;
		}
	}

	return ret;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Generate decode table
void b64_generate_decode_table()
{
	int    inv[80];
	size_t i;

	memset(inv, -1, sizeof(inv));
	for (i=0; i<sizeof(b64)-1; i++) {
		inv[b64[i]-43] = i;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// check if char is valid
int b64_is_valid_char(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	if (c >= 'A' && c <= 'Z')
		return 1;
	if (c >= 'a' && c <= 'z')
		return 1;
	if (c == '+' || c == '/' || c == '=')
		return 1;
	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Decode
int b64_decode(const char *in, unsigned char *out, size_t outlen)
{
	size_t len;
	size_t i;
	size_t j;
	int    v;

	if (in == NULL || out == NULL)
		return 0;

	len = strlen(in);
	if (outlen < b64_decoded_size(in) || len % 4 != 0)
		return 0;

	for (i=0; i<len; i++) {
		if (!b64_is_valid_char(in[i])) {
			return 0;
		}
	}

	for (i=0, j=0; i<len; i+=4, j+=3) {
		v = b64invs[in[i]-43];
		v = (v << 6) | b64invs[in[i+1]-43];
		v = in[i+2]=='=' ? v << 6 : (v << 6) | b64invs[in[i+2]-43];
		v = in[i+3]=='=' ? v << 6 : (v << 6) | b64invs[in[i+3]-43];

		out[j] = (v >> 16) & 0xFF;
		if (in[i+2] != '=')
			out[j+1] = (v >> 8) & 0xFF;
		if (in[i+3] != '=')
			out[j+2] = v & 0xFF;
	}

	return 1;
}



