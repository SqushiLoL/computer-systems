#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

uint32_t extractBits(uint32_t value, int end_bit, int start_bit);
int32_t sign_extend32(int32_t value, int bits);

#endif
