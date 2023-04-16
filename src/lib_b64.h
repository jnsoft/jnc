#include <stdlib.h>
#include <string.h>

/*****************************BEGIN SECTION BASE64****************************/

/*
In Base64 encoding, 3 binary bytes are represented as 4 characters.
There are different implementations of base64 which differ for the last two characters 
and line ending requirements. The first 62 characters are always A-Za-z0-9.
PEM and MIME encoding are the most common implementaion and use "+/" as the last two characters. 
*/
const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 To get the number of base64 characters for given input:
 Take length of input rounded up to the nearest multiple of 3. 
 Then, divide by 3 to get the number of 3 byte blocks. 
 Multiply by 4 to get the number of base64 characters.
 */
size_t b64_encoded_size(size_t inlen)
{
	size_t ret = inlen;
	if (inlen % 3 != 0)
		ret += 3 - (inlen % 3);
	return (ret/3) * 4;
}

/* Reverse of b64_encoded_size: map 4 bytes into 3 bytes */
size_t b64_decoded_size(const char *in)
{
	if (in == NULL)
		return 0;

	size_t len = strlen(in);
	size_t ret = len / 4 * 3;

    // adjust for padding
	for (size_t i=len; i-->0; ) {
		if (in[i] == '=') 
			ret--;
		else 
			break;
	}
	return ret;
}

/*
Put 3 bytes of input into 4 bytes of encoded bytes,
use '=' for padding the last sequence to a multiple of 3
*/
char *b64_encode(const unsigned char *in, size_t len)
{
	char    *out;
	size_t  v;

	if (in == NULL || len == 0) 
        return NULL;

	size_t elen = b64_encoded_size(len);
	out  = (char *)malloc(elen+1);
	out[elen] = '\0'; // null terminated string

	for (size_t i=0, j=0; i<len; i+=3, j+=4) {
		 // Push 3 bytes into an int, if there are less than 3 bytes,
         // shift so we can properly pull the value back out later.
        v = in[i]; 
		v = i+1 < len ? v << 8 | in[i+1] : v << 8;
		v = i+2 < len ? v << 8 | in[i+2] : v << 8; 
        
        // Pop 4 bytes (0x3F is a 6-bit mask)
		out[j]   = b64chars[(v >> 18) & 0x3F];
		out[j+1] = b64chars[(v >> 12) & 0x3F];
		if (i+1 < len)
			out[j+2] = b64chars[(v >> 6) & 0x3F];
		else 
			out[j+2] = '=';
		if (i+2 < len)
			out[j+3] = b64chars[v & 0x3F];
		else
			out[j+3] = '=';
	}

	return out;
}

char *b64_encode_wrapper(const unsigned char *in) {
    return b64_encode(in, strlen((const char*)in));
}

/*
Generated table is a shift from + (lowest ASCII character, 43) in the sequence. 
The table index is the value in b64chars 
and the table value is the index of the vale in b64chars. 
-1 is used as a placeholder for values that are not in the table.
*/
int * b64_generate_decode_table()
{
	static int inv[80];
	
	memset(inv, -1, sizeof(inv));
	for (size_t i=0; i<sizeof(b64chars)-1; i++)
		inv[b64chars[i]-43] = i;
    
    return inv;
}

/*
TODO: expand to handle newlines, deal with line length requirements, 
ignore whitespace if necessary, and verify there are two or less = , 
and = is only present at the end.
*/
int b64_isvalidchar(char c)
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

/*
Decode the sets of 4 base64 characters into 3 bytes. 
This handles the last grouping of bytes not being a multiple of 3.
*/
unsigned char * b64_decode(const char *in)
{
    int v;
    int *inv_ptr = b64_generate_decode_table();
    size_t outlen = b64_decoded_size(in)+1; // +1 for the NULL terminator.

	unsigned char *out = (unsigned char *)malloc(outlen);

	if (in == NULL || out == NULL)
		return NULL;

	size_t len = strlen(in);
	if (outlen < b64_decoded_size(in) || len % 4 != 0)
		return NULL;

	for (size_t i=0; i<len; i++) 
		if (!b64_isvalidchar(in[i]))
			return NULL;

	for (size_t i=0, j=0; i<len; i+=4, j+=3) {
        /*
        Push four characters decoded into an int that will hold our 3 bytes. 
        Shift to put the preceding bytes into the correct position within the int.
        */
		v = inv_ptr[in[i]-43];
		v = (v << 6) | inv_ptr[in[i+1]-43];
        // charachters three and four can be padding
		v = in[i+2]=='=' ? v << 6 : (v << 6) | inv_ptr[in[i+2]-43];
		v = in[i+3]=='=' ? v << 6 : (v << 6) | inv_ptr[in[i+3]-43];

        // 3 total bytes can be pulled out of the int, but don't pull padding
		out[j] = (v >> 16) & 0xFF; // 0xFF is the bit-mask
		if (in[i+2] != '=')
			out[j+1] = (v >> 8) & 0xFF;
		if (in[i+3] != '=')
			out[j+2] = v & 0xFF;
	}

	out[outlen] = '\0';
    return out;
}

/*****************************END SECTION BASE64**********************************/