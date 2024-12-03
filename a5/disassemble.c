#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disassemble.h"

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
  imm_110  = extractBits(instruction, 31, 20); // for I-type
  imm_40   = extractBits(instruction, 11, 7);  // for S-type
  imm_3112 = extractBits(instruction, 31, 12); // for U-type

  // check for R-type
  if (opcode == 0b0110011) {
    // ADD
    if (funct3 == 0b000 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "add x%d, x%d, x%d", rd, rs1, rs2);
      // SUB
    } else if (funct3 == 0b000 && funct7 == 0b0100000) {
      snprintf(result, buf_size, "sub x%d, x%d, x%d", rd, rs1, rs2);
      // SLL
    } else if (funct3 == 0b001 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "sll x%d, x%d, x%d", rd, rs1, rs2);
      // SLT
    } else if (funct3 == 0b010 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "slt x%d, x%d, x%d", rd, rs1, rs2);
      // SLTU
    } else if (funct3 == 0b011 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "sltu x%d, x%d, x%d", rd, rs1, rs2);
      // XOR
    } else if (funct3 == 0b100 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "xor x%d, x%d, x%d", rd, rs1, rs2);
      // SRL
    } else if (funct3 == 0b101 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "srl x%d, x%d, x%d", rd, rs1, rs2);
      // SRA
    } else if (funct3 == 0b101 && funct7 == 0b0100000) {
      snprintf(result, buf_size, "sra x%d, x%d, x%d", rd, rs1, rs2);
      // OR
    } else if (funct3 == 0b110 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "or x%d, x%d, x%d", rd, rs1, rs2);
      // AND
    } else if (funct3 == 0b111 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "and x%d, x%d, x%d", rd, rs1, rs2);
    } else {
      snprintf(result, buf_size, "unknown");
    }
  }

  // check for I-type
  // JALR
  else if (opcode == 0b1100111) {
    snprintf(result, buf_size, "jalr x%d, %d(x%d)", rd, imm_110, rs1);
  } else if (opcode == 0b0000011) {
    // LB
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "lb x%d, %d(x%d)", rd, imm_110, rs1);
      // LH
    } else if (funct3 == 0b001) {
      snprintf(result, buf_size, "lh x%d, %d(x%d)", rd, imm_110, rs1);
      // LW
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "lw x%d, %d(x%d)", rd, imm_110, rs1);
      // LBU
    } else if (funct3 == 0b100) {
      snprintf(result, buf_size, "lbu x%d, %d(x%d)", rd, imm_110, rs1);
      // LHU
    } else if (funct3 == 0b101) {
      snprintf(result, buf_size, "lhu x%d, %d(x%d)", rd, imm_110, rs1);
    } else {
      snprintf(result, buf_size, "unknown");
    }
  } else if (opcode == 0b0010011) {
    // ADDI
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "addi x%d, x%d, %d", rd, rs1, imm_110);
      // SLTI
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "slti x%d, x%d, %d", rd, rs1, imm_110);
      // SLTIU
    } else if (funct3 == 0b011) {
      snprintf(result, buf_size, "sltiu x%d, x%d, %d", rd, rs1, imm_110);
      // XORI
    } else if (funct3 == 0b100) {
      snprintf(result, buf_size, "xori x%d, x%d, %d", rd, rs1, imm_110);
      // ORI
    } else if (funct3 == 0b110) {
      snprintf(result, buf_size, "ori x%d, x%d, %d", rd, rs1, imm_110);
      // ANDI
    } else if (funct3 == 0b111) {
      snprintf(result, buf_size, "andi x%d, x%d, %d", rd, rs1, imm_110);
    } else {
      snprintf(result, buf_size, "unknown");
    }
  }

  // check for S-type
  else if (opcode == 0b0100011) {
    // SB
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "sb x%d, %d(x%d)", rs2, imm_40, rs1);
      // SH
    } else if (funct3 == 0b001) {
      snprintf(result, buf_size, "sh x%d, %d(x%d)", rs2, imm_40, rs1);
      // SW
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "sw x%d, %d(x%d)", rs2, imm_40, rs1);
    } else {
      snprintf(result, buf_size, "unknown");
    }
  }

  // check for U-type
  // LUI
  else if (opcode == 0b0110111) {
    snprintf(result, buf_size, "lui x%d, %d", rd, imm_3112);
    // AUIPC
  } else if (opcode == 0b0010111) {
    snprintf(result, buf_size, "auipc x%d, %d", rd, imm_3112);
  } else {
    snprintf(result, buf_size, "unknown");
  }
}