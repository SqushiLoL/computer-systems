#include "disassemble.h"
#include "tools.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void disassemble(uint32_t addr, uint32_t instruction, char* result,
                 size_t buf_size, struct symbols* symbols) {

  // Suppress unused parameter warning
  (void)addr;
  (void)symbols;

  uint32_t opcode, rd, funct3, rs1, rs2, funct7, funct12;
  int32_t  imm_110, imm_40, imm_3112;

  opcode = extractBits(instruction, 6, 0);
  // printf("opcode: %d\n", opcode);
  rd = extractBits(instruction, 11, 7);
  // printf("rd: %d\n", rd);
  funct3 = extractBits(instruction, 14, 12);
  // printf("funct3: %d\n", funct3);
  rs1 = extractBits(instruction, 19, 15);
  // printf("rs1: %d\n", rs1);
  rs2 = extractBits(instruction, 24, 20);
  // printf("rs2: %d\n", rs2);
  funct7 = extractBits(instruction, 31, 25);
  // printf("funct7: %d\n", funct7);
  funct12 = extractBits(instruction, 31, 20);

  // Immediate fields
  imm_110 = sign_extend32(extractBits(instruction, 31, 20), 12);
  // printf("imm_110: %d\n", imm_110);
  imm_40 = sign_extend32((extractBits(instruction, 11, 7) |
                          (extractBits(instruction, 31, 25) << 5)),
                         12);
  // printf("imm_40: %d\n", imm_40);
  imm_3112 = sign_extend32(extractBits(instruction, 31, 12), 20);
  // printf("imm_3112: %d\n", imm_3112);

  // check for R-type
  if (opcode == 0b0110011) {
    // ADD
    if (funct3 == 0b000 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "add x%d, x%d, x%d", rd, rs1, rs2);
    }
    // SUB
    else if (funct3 == 0b000 && funct7 == 0b0100000) {
      snprintf(result, buf_size, "sub x%d, x%d, x%d", rd, rs1, rs2);
    }
    // SLL
    else if (funct3 == 0b001 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "sll x%d, x%d, x%d", rd, rs1, rs2);
    }
    // SLT
    else if (funct3 == 0b010 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "slt x%d, x%d, x%d", rd, rs1, rs2);
    }
    // SLTU
    else if (funct3 == 0b011 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "sltu x%d, x%d, x%d", rd, rs1, rs2);
    }
    // XOR
    else if (funct3 == 0b100 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "xor x%d, x%d, x%d", rd, rs1, rs2);
    }
    // SRL
    else if (funct3 == 0b101 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "srl x%d, x%d, x%d", rd, rs1, rs2);
    }
    // SRA
    else if (funct3 == 0b101 && funct7 == 0b0100000) {
      snprintf(result, buf_size, "sra x%d, x%d, x%d", rd, rs1, rs2);
    }
    // OR
    else if (funct3 == 0b110 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "or x%d, x%d, x%d", rd, rs1, rs2);
    }
    // AND
    else if (funct3 == 0b111 && funct7 == 0b0000000) {
      snprintf(result, buf_size, "and x%d, x%d, x%d", rd, rs1, rs2);
    }
    // RV32M extension (e.g., MUL, DIV)
    else if (funct7 == 0b0000001) {
      // MUL
      if (funct3 == 0b000) {
        snprintf(result, buf_size, "mul x%d, x%d, x%d", rd, rs1, rs2);
      }
      // MULH
      else if (funct3 == 0b001) {
        snprintf(result, buf_size, "mulh x%d, x%d, x%d", rd, rs1, rs2);
      }
      // MULHSU
      else if (funct3 == 0b010) {
        snprintf(result, buf_size, "mulhsu x%d, x%d, x%d", rd, rs1, rs2);
      }
      // MULHU
      else if (funct3 == 0b011) {
        snprintf(result, buf_size, "mulhu x%d, x%d, x%d", rd, rs1, rs2);
      }
      // DIV
      else if (funct3 == 0b100) {
        snprintf(result, buf_size, "div x%d, x%d, x%d", rd, rs1, rs2);
      }
      // DIVU
      else if (funct3 == 0b101) {
        snprintf(result, buf_size, "divu x%d, x%d, x%d", rd, rs1, rs2);
      }
      // REM
      else if (funct3 == 0b110) {
        snprintf(result, buf_size, "rem x%d, x%d, x%d", rd, rs1, rs2);
      }
      // REMU
      else if (funct3 == 0b111) {
        snprintf(result, buf_size, "remu x%d, x%d, x%d", rd, rs1, rs2);
      } else {
        snprintf(result, buf_size, "unknown RV32M R-type");
      }
    } else {
      snprintf(result, buf_size,
               "unknown R-type: (opcode=0xx%d, funct3=%d, funct7=%d)", opcode,
               funct3, funct7);
    }
  }

  else if (opcode == 0b1100011) {
    sign_extend32(imm_40, 12);
    if (funct3 == 0b000) {
      // BEQ
      snprintf(result, buf_size, "beq x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b001) {
      // BNE
      snprintf(result, buf_size, "bne x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b100) {
      // BLT
      snprintf(result, buf_size, "blt x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b101) {
      // BGE
      snprintf(result, buf_size, "bge x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b110) {
      // BLTU
      snprintf(result, buf_size, "bltu x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b111) {
      // BGEU
      snprintf(result, buf_size, "bgeu x%d, x%d, %d", rs1, rs2, imm_40);
    } else {
      snprintf(result, buf_size, "unknown extension R-type");
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
      snprintf(result, buf_size, "unknown I-type (load)");
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
    } else if (funct3 == 0b001) {
      // SLLI
      if (funct7 == 0b0000000) {
        snprintf(result, buf_size, "slli x%d, x%d, %d", rd, rs1, imm_110);
      } else {
        snprintf(result, buf_size, "unknown I-type (shift left)");
      }
    } else if (funct3 == 0b101) {
      // SRLI
      if (funct7 == 0b0000000) {
        snprintf(result, buf_size, "srli x%d, x%d, %d", rd, rs1, imm_110);
      }
      // SRAI
      else if (funct7 == 0b0100000) {
        snprintf(result, buf_size, "srai x%d, x%d, %d", rd, rs1, imm_110);
      } else {
        snprintf(result, buf_size, "unknown I-type (right shift)");
      }

    } else {
      snprintf(result, buf_size, "unknown I-type ");
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
      snprintf(result, buf_size, "unknown S-type");
    }
  }

  // check for U-type
  // LUI
  else if (opcode == 0b0110111) {
    snprintf(result, buf_size, "lui x%d, %d", rd, imm_3112);
    // AUIPC
  } else if (opcode == 0b0010111) {
    snprintf(result, buf_size, "auipc x%d, %d", rd, imm_3112);
  }

  // check for J-type
  // JAL
  else if (opcode == 0b1101111) {
    snprintf(result, buf_size, "jal x%d, %d", rd, imm_3112);
  }

  // check for B-type
  else if (opcode == 0b1100011) {
    if (funct3 == 0b000) {
      // BEQ
      snprintf(result, buf_size, "beq x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b001) {
      // BNE
      snprintf(result, buf_size, "bne x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b100) {
      // BLT
      snprintf(result, buf_size, "blt x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b101) {
      // BGE
      snprintf(result, buf_size, "bge x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b110) {
      // BLTU
      snprintf(result, buf_size, "bltu x%d, x%d, %d", rs1, rs2, imm_40);
    } else if (funct3 == 0b111) {
      // BGEU
      snprintf(result, buf_size, "bgeu x%d, x%d, %d", rs1, rs2, imm_40);
    } else {
      snprintf(result, buf_size, "unknown B-type");
    }

    // ??? - type
    // ecall
  } else if (opcode == 0b1110011 && funct3 == 0b000 &&
             funct12 == 0b000000000000) {
    snprintf(result, buf_size, "ecall");
  } else {
    snprintf(result, buf_size, "unknown");
  }
}
