#pragma once

#include <stdint.h>

#define LOOKUP_CAPS "0123456789ABCDEF"
#define LOOKUP_LOWER "0123456789abcdef"

void hex2bin(char *_des, const char *pstr, uint32_t length);

void bin2hex(char *_des, const char *pstr, uint32_t length,
             const char *lookup = LOOKUP_CAPS);
