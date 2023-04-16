#include <stdio.h>

// macro for (static) arrays, don't use on pointers to array
#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

void print_char(char c){
	int i;
  	for (i = 0; i < 8; i++)
      printf("%d", !!((c << i) & 0x80));
}