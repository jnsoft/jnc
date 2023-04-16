#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// macro for (static) arrays, don't use on pointers to array
#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif
