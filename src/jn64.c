#include <stdlib.h>
#include <stdio.h>
#include "lib_hex.h"
#include "lib_b64.h"



const char usageMsg[] = "jn64\n" \
		"Usage: [-e|-d] [-i|-o] [encode] [decode] [input] [output]\n" \
		"   Where [-e] encode (to base64),\n" \
		"         [-d] decode (from base64),\n" \
		"         [-i] specifies input file, otherwise use text input,\n" \
		"         [-o] specifies output file, otherwise use text output.\n";



const char* argopt(int argc, const char * const *argv, char key){

    for(int i=1; i<argc; i++){
        const char *c = argv[i];
        if(*c!='-') 
			continue;
        while(*++c) 
			if(*c==key) 
				return argv[(i+1)%argc];
    }

    return 0;
}

int main(int argc, char **argv)
{
	unsigned char msg[] = "This is message!";
	unsigned char key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	
	char FLAGS = 0;
	const char *input = argopt(argc, (const char *const *)argv, 'i');
	const char *output = argopt(argc, (const char *const *)argv, 'o');

	if(argopt(argc, (const char *const *)argv, 'e'))
		setbit(&FLAGS,0);
	if(argopt(argc, (const char *const *)argv, 'd'))
		setbit(&FLAGS,1);
	if(input)
		setbit(&FLAGS,2);
	if(output)
		setbit(&FLAGS,3);
	if(argopt(argc, (const char *const *)argv, 'h')){
		printf(usageMsg);
		return 0;
	}
	if(argc == 1){
		printf(usageMsg);
		return 0;
	}

	if(BIT_CHECK(FLAGS,0) & BIT_CHECK(FLAGS,1)){
		printf("Error: use encode (-e) or decode (-d), not both\n");
		return 0;
	}

	// printf("No of arguments: %i\n", argc); // DEBUG

	const char *data = argv[argc-1];
	if(data[0]!='-')
	{
		if(BIT_CHECK(FLAGS,1)){
			char *decoded = b64_decode(data);
			if (decoded == NULL) {
				printf("Decode Failure\n");
				return -1;
			}
			printf("%s\n", decoded);
			free(decoded);
		}
		else{
			char *enc = b64_encode_wrapper((const unsigned char *)data);
			printf("%s\n", enc);
			free(enc);
		}		
	}

	return 0;	
}
