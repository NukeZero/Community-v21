#ifndef __FLDES_H__
#define __FLDES_H__


#include "FLCryptoCommon.h"

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
	uint32 esk[32];     /* DES encryption subkeys */
	uint32 dsk[32];     /* DES decryption subkeys */
}
FL_des_context;

typedef struct
{
	uint32 esk[96];     /* Triple-DES encryption subkeys */
	uint32 dsk[96];     /* Triple-DES decryption subkeys */
}
FL_des3_context;

int  des_set_key( FL_des_context *ctx, uint8 key[8] );
void des_encrypt( FL_des_context *ctx, uint8 input[8], uint8 output[8] );
void des_decrypt( FL_des_context *ctx, uint8 input[8], uint8 output[8] );

int  des3_set_2keys( FL_des3_context *ctx, uint8 key1[8], uint8 key2[8] );
int  des3_set_3keys( FL_des3_context *ctx, uint8 key1[8], uint8 key2[8],
					uint8 key3[8] );

void des3_encrypt( FL_des3_context *ctx, uint8 input[8], uint8 output[8] );
void des3_decrypt( FL_des3_context *ctx, uint8 input[8], uint8 output[8] );


#endif // __FLDES_H__
