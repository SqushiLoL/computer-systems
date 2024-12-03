#include "disassemble.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t extractBits(uint32_t value, int end_bit, int start_bit) {
  int num_bits = end_bit - start_bit + 1;
  return (value >> start_bit) & ((1 << num_bits) - 1);
}

void disassemble(uint32_t addr, uint32_t instruction, char* result,
                 size_t buf_size, struct symbols* symbols) {

  uint32_t opcode, rd, funct3, rs1, rs2, funct7, imm_110, imm_115, imm_40,
      imm_3112;

  opcode   = extractBits(instruction, 0, 6);
  rd       = extractBits(instruction, 11, 7);
  funct3   = extractBits(instruction, 14, 12);
  rs1      = extractBits(instruction, 19, 15);
  rs2      = extractBits(instruction, 24, 20);
  funct7   = extractBits(instruction, 31, 25);
  imm_110  = extractBits(instruction, 31, 20);
  imm_40   = extractBits(instruction, 11, 7);
  imm_3112 = extractBits(instruction, 31, 12);

  // check for R-type
  if (opcode == 0b0110011) {
    // ADD
    if (funct3 == 0b000 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "add x%d, x%d, x%d", rd, rs1, rs2);
      // SUB
    } else if (funct3 == 0b000 && funct7 == 0b0100000) {
      snprintf(result, buf_size, "sub x%d, x%d, x%d", rd, rs1, rs2);
    } else {
      snprintf(result, buf_size, "unknown");
    }
  }

  // check for I-type

  // check for U-type

  // check for B-type
}