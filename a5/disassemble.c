#include "disassemble.h"
#include "tools.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// sign_extend32 & extractBits in "tools.h"

void disassemble(uint32_t addr, uint32_t instruction, char* result,
                 size_t buf_size, struct symbols* symbols) {

  (void)addr;    // remove warning
  (void)symbols; // remove warning

  uint32_t opcode, rd, funct3, rs1, rs2, funct7, funct12;
  int32_t  i_imm, s_imm, u_imm, jal_imm, b_imm;

  opcode  = extractBits(instruction, 6, 0);
  rd      = extractBits(instruction, 11, 7);
  funct3  = extractBits(instruction, 14, 12);
  rs1     = extractBits(instruction, 19, 15);
  rs2     = extractBits(instruction, 24, 20);
  funct7  = extractBits(instruction, 31, 25);
  funct12 = extractBits(instruction, 31, 20);

  // Immediate fields
  i_imm = extractBits(instruction, 31, 20);
  i_imm = sign_extend32(i_imm, 12);

  s_imm = (extractBits(instruction, 11, 7) |
           (extractBits(instruction, 31, 25) << 5));
  s_imm = sign_extend32(s_imm, 12);

  u_imm = extractBits(instruction, 31, 12);
  u_imm = sign_extend32(u_imm, 20);

  jal_imm = (extractBits(instruction, 19, 12) << 12) |
            (extractBits(instruction, 20, 20) << 11) |
            (extractBits(instruction, 30, 21) << 1) |
            (extractBits(instruction, 31, 31) << 20);
  jal_imm = sign_extend32(jal_imm, 21);

  b_imm = (extractBits(instruction, 31, 31) << 12) |
          (extractBits(instruction, 7, 7) << 11) |
          (extractBits(instruction, 30, 25) << 5) |
          (extractBits(instruction, 11, 8) << 1);
  b_imm = sign_extend32(b_imm, 13);

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
    // RV32M extension
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

  // check for I-type
  // JALR
  else if (opcode == 0b1100111) {
    snprintf(result, buf_size, "jalr x%d, %d(x%d)", rd, i_imm, rs1);
  } else if (opcode == 0b0000011) {
    // LB
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "lb x%d, %d(x%d)", rd, i_imm, rs1);
      // LH
    } else if (funct3 == 0b001) {
      snprintf(result, buf_size, "lh x%d, %d(x%d)", rd, i_imm, rs1);
      // LW
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "lw x%d, %d(x%d)", rd, i_imm, rs1);
      // LBU
    } else if (funct3 == 0b100) {
      snprintf(result, buf_size, "lbu x%d, %d(x%d)", rd, i_imm, rs1);
      // LHU
    } else if (funct3 == 0b101) {
      snprintf(result, buf_size, "lhu x%d, %d(x%d)", rd, i_imm, rs1);
    } else {
      snprintf(result, buf_size, "unknown I-type (load)");
    }
  } else if (opcode == 0b0010011) {
    // ADDI
    if (funct3 == 0b000) {
      snprintf(result, buf_size, "addi x%d, x%d, %d", rd, rs1, i_imm);
      // SLTI
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "slti x%d, x%d, %d", rd, rs1, i_imm);
      // SLTIU
    } else if (funct3 == 0b011) {
      snprintf(result, buf_size, "sltiu x%d, x%d, %d", rd, rs1, i_imm);
      // XORI
    } else if (funct3 == 0b100) {
      snprintf(result, buf_size, "xori x%d, x%d, %d", rd, rs1, i_imm);
      // ORI
    } else if (funct3 == 0b110) {
      snprintf(result, buf_size, "ori x%d, x%d, %d", rd, rs1, i_imm);
      // ANDI
    } else if (funct3 == 0b111) {
      snprintf(result, buf_size, "andi x%d, x%d, %d", rd, rs1, i_imm);
    } else if (funct3 == 0b001) {
      // SLLI
      if (funct7 == 0b0000000) {
        uint32_t shamt = extractBits(instruction, 24, 20);
        snprintf(result, buf_size, "slli x%d, x%d, %u", rd, rs1, shamt);
      } else {
        snprintf(result, buf_size, "unknown I-type (shift left)");
      }
    } else if (funct3 == 0b101) {
      // SRLI
      if (funct7 == 0b0000000) {
        uint32_t shamt = extractBits(instruction, 24, 20);
        snprintf(result, buf_size, "srli x%d, x%d, %d", rd, rs1, shamt);
      }
      // SRAI
      else if (funct7 == 0b0100000) {
        uint32_t shamt = extractBits(instruction, 24, 20);
        snprintf(result, buf_size, "srai x%d, x%d, %d", rd, rs1, shamt);
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
      snprintf(result, buf_size, "sb x%d, %d(x%d)", rs2, s_imm, rs1);
      // SH
    } else if (funct3 == 0b001) {
      snprintf(result, buf_size, "sh x%d, %d(x%d)", rs2, s_imm, rs1);
      // SW
    } else if (funct3 == 0b010) {
      snprintf(result, buf_size, "sw x%d, %d(x%d)", rs2, s_imm, rs1);
    } else {
      snprintf(result, buf_size, "unknown S-type");
    }
  }

  // check for U-type
  // LUI
  else if (opcode == 0b0110111) {
    snprintf(result, buf_size, "lui x%d, %d", rd, u_imm);
    // AUIPC
  } else if (opcode == 0b0010111) {
    snprintf(result, buf_size, "auipc x%d, %d", rd, u_imm);
  }

  // check for J-type
  // JAL
  else if (opcode == 0b1101111) {
    uint32_t target = addr + jal_imm;
    snprintf(result, buf_size, "jal x%d, %x", rd, target);
  }

  // check for B-type
  else if (opcode == 0b1100011) {
    uint32_t branch_tar = addr + b_imm;
    if (funct3 == 0b000) {
      // BEQ
      snprintf(result, buf_size, "beq x%d, x%d, %x", rs1, rs2, branch_tar);
    } else if (funct3 == 0b001) {
      // BNE
      snprintf(result, buf_size, "bne x%d, x%d, %x", rs1, rs2, branch_tar);
    } else if (funct3 == 0b100) {
      // BLT
      snprintf(result, buf_size, "blt x%d, x%d, %x", rs1, rs2, branch_tar);
    } else if (funct3 == 0b101) {
      // BGE
      snprintf(result, buf_size, "bge x%d, x%d, %x", rs1, rs2, branch_tar);
    } else if (funct3 == 0b110) {
      // BLTU
      snprintf(result, buf_size, "bltu x%d, x%d, %x", rs1, rs2, branch_tar);
    } else if (funct3 == 0b111) {
      // BGEU
      snprintf(result, buf_size, "bgeu x%d, x%d, %x", rs1, rs2, branch_tar);
    } else {
      snprintf(result, buf_size, "unknown B-type");
    }

    // system calls
    // ecall
  } else if (opcode == 0b1110011 && funct3 == 0b000 &&
             funct12 == 0b000000000000) {
    snprintf(result, buf_size, "ecall");
  } else {
    snprintf(result, buf_size, "unknown");
  }
}
