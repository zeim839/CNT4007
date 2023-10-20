#ifndef CNT4007_SYSTEM_HPP
#define CNT4007_SYSTEM_HPP

typedef unsigned char Byte;  // 1 byte.
typedef unsigned int  Quad;  // 4 bytes.

#include <stdio.h>

// Assert that the condition is true, or throw an error and kill the
// process.
#define ASSERT(cond, ...)						\
{								        \
	if (!(cond)) {							\
		printf("ERROR [%s:%d] ", __FILE__, __LINE__);		\
		printf(__VA_ARGS__);					\
		printf("\n");						\
		exit(1);						\
	}								\
}

#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#endif // CNT4007_SYSTEM_HPP
