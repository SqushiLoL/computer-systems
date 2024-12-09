#include "tools.h"

uint32_t extractBits(uint32_t value, int end_bit, int start_bit) {
  if (start_bit > end_bit) {
    int temp  = start_bit;
    start_bit = end_bit;
    end_bit   = temp;
  }
  int num_bits = end_bit - start_bit + 1;
  return (value >> start_bit) & ((1U << num_bits) - 1);
}

int32_t sign_extend32(int32_t value, int bits) {
  int shift = 32 - bits;
  return (value << shift) >> shift;
}
