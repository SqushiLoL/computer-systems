#include "disassemble.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* uint32_t extractBits(uint32_t value, int end_bit, int start_bit) {
  int num_bits = end_bit - start_bit + 1;
  return (value >> start_bit) & ((1 << num_bits) - 1);
} */
uint32_t extractBits(uint32_t value, int end_bit, int start_bit) {
  if (start_bit > end_bit) {
    int temp  = start_bit;
    start_bit = end_bit;
    end_bit   = temp;
  }
  int num_bits = end_bit - start_bit + 1;
  return (value >> start_bit) & ((1U << num_bits) - 1);
}

const char* register_names[32] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

int32_t sign_extend32(int32_t value, int bits) {
  int shift = 32 - bits;
  return (value << shift) >> shift;
}

int32_t sign_extend12(int32_t value, int bits) {
  int shift = 12 - bits;
  return (value << shift) >> shift;
}

void disassemble(uint32_t addr, uint32_t instruction, char* result,
                 size_t buf_size, struct symbols* symbols) {

  uint32_t opcode, rd, funct3, rs1, rs2, funct7, imm_110, imm_115, imm_40,
      imm_3112;

  // instruction = memory_rd_w(instruction, addr);

  opcode = extractBits(instruction, 6, 0);
  printf("Opcode %d\n", opcode);
  rd = extractBits(instruction, 11, 7);
  printf("rd %d\n", rd);
  funct3 = extractBits(instruction, 14, 12);
  printf("f3 %d\n", funct3);
  rs1 = extractBits(instruction, 19, 15);
  printf("rs1 %d\n", rs1);
  rs2 = extractBits(instruction, 24, 20);
  printf("rs2 %d\n", rs2);
  funct7 = extractBits(instruction, 31, 25);
  printf("funct7 %d\n", funct7);
  imm_110 = extractBits(instruction, 31, 20);
  printf("imm110 %d\n", imm_110);
  imm_40 = extractBits(instruction, 11, 7);
  printf("imm40 %d\n", imm_40);
  imm_3112 = extractBits(instruction, 31, 12);
  printf("imm3112 %d\n", imm_3112);

  // check for R-type
  if (opcode == 0b0110011) {
    // ADD
    if (funct3 == 0b000 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "add %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // SUB
    } else if (funct3 == 0b000 && funct7 == 0b0100000) {
      snprintf(result, buf_size, "sub %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // SLL
    } else if (funct3 == 0b001 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "sll %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // SLT
    } else if (funct3 == 0b010 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "slt %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // SLTU
    } else if (funct3 == 0b011 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "sltu %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // XOR
    } else if (funct3 == 0b100 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "xor %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // SRL
    } else if (funct3 == 0b101 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "srl %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // SRA
    } else if (funct3 == 0b101 && funct7 == 0b0100000) {
      snprintf(result, buf_size, "sra %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // OR
    } else if (funct3 == 0b110 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "or %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
      // AND
    } else if (funct3 == 0b111 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "and %s, %s, %s", register_names[rd],
               register_names[rs1], register_names[rs2]);
    } else {
      snprintf(result, buf_size, "unknown Standard R-type");
    }
  }

  else if (opcode == 0b1100011) {
    sign_extend32(imm_40, 12);
    if (funct3 == 0b000) {
      // BEQ
      snprintf(result, buf_size, "beq %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b001) {
      // BNE
      snprintf(result, buf_size, "bne %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b100) {
      // BLT
      snprintf(result, buf_size, "blt %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b101) {
      // BGE
      snprintf(result, buf_size, "bge %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b110) {
      // BLTU
      snprintf(result, buf_size, "bltu %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b111) {
      // BGEU
      snprintf(result, buf_size, "bgeu %s, %s, %d", rs1, rs2, imm_40);
    } else {
      snprintf(result, buf_size, "unknown extension R-type");
    }
  }

  // check for I-type
  // JALR
  else if (opcode == 0b1100111) {
    snprintf(result, buf_size, "jalr %s, %d(%s)", rd, imm_110, rs1);
  } else if (opcode == 0b0000011) {
    // LB
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "lb %s, %d(%s)", rd, imm_110, rs1);
      // LH
    } else if (funct3 == 0b001) {
      snprintf(result, buf_size, "lh %s, %d(%s)", rd, imm_110, rs1);
      // LW
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "lw %s, %d(%s)", rd, imm_110, rs1);
      // LBU
    } else if (funct3 == 0b100) {
      snprintf(result, buf_size, "lbu %s, %d(%s)", rd, imm_110, rs1);
      // LHU
    } else if (funct3 == 0b101) {
      snprintf(result, buf_size, "lhu %s, %d(%s)", rd, imm_110, rs1);
    } else {
      snprintf(result, buf_size, "unknown I-type (load)");
    }
  } else if (opcode == 0b0010011) {
    // ADDI
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "addi %s, %s, %d", rd, rs1, imm_110);
      // SLTI
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "slti %s, %s, %d", rd, rs1, imm_110);
      // SLTIU
    } else if (funct3 == 0b011) {
      snprintf(result, buf_size, "sltiu %s, %s, %d", rd, rs1, imm_110);
      // XORI
    } else if (funct3 == 0b100) {
      snprintf(result, buf_size, "xori %s, %s, %d", rd, rs1, imm_110);
      // ORI
    } else if (funct3 == 0b110) {
      snprintf(result, buf_size, "ori %s, %s, %d", rd, rs1, imm_110);
      // ANDI
    } else if (funct3 == 0b111) {
      snprintf(result, buf_size, "andi %s, %s, %d", rd, rs1, imm_110);
    } else {
      snprintf(result, buf_size, "unknown I-type");
    }
  }

  // check for S-type
  else if (opcode == 0b0100011) {
    // SB
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "sb %s, %d(%s)", rs2, imm_40, rs1);
      // SH
    } else if (funct3 == 0b001) {
      snprintf(result, buf_size, "sh %s, %d(%s)", rs2, imm_40, rs1);
      // SW
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "sw %s, %d(%s)", rs2, imm_40, rs1);
    } else {
      snprintf(result, buf_size, "unknown S-type");
    }
  }

  // check for U-type
  // LUI
  else if (opcode == 0b0110111) {
    snprintf(result, buf_size, "lui %s, %d", rd, imm_3112);
    // AUIPC
  } else if (opcode == 0b0010111) {
    snprintf(result, buf_size, "auipc %s, %d", rd, imm_3112);
  }

  // check for J-type
  // JAL
  else if (opcode == 0b1101111) {
    snprintf(result, buf_size, "jal %s, %d", rd, imm_3112);
  }

  // check for B-type
  else if (opcode == 0b1100011) {
    if (funct3 == 0b000) {
      // BEQ
      snprintf(result, buf_size, "beq %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b001) {
      // BNE
      snprintf(result, buf_size, "bne %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b100) {
      // BLT
      snprintf(result, buf_size, "blt %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b101) {
      // BGE
      snprintf(result, buf_size, "bge %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b110) {
      // BLTU
      snprintf(result, buf_size, "bltu %s, %s, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b111) {
      // BGEU
      snprintf(result, buf_size, "bgeu %s, %s, %d", rs1, rs2, imm_40);
    } else {
      snprintf(result, buf_size, "unknown B-type");
    }
  }
}
