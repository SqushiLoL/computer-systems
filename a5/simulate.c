#include "simulate.h"
#include "tools.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Stat simulate(struct memory* mem, int start_addr, FILE* log_file,
                     struct symbols* symbols) {
  (void)symbols;                       // remove warning
  uint32_t registers[32] = {0};        // registers
  uint32_t program_count = start_addr; // Start simulation from entry point
  uint32_t instruction;
  uint32_t insns = 0;

  while (1) {
    // Fetch instruction
    instruction = memory_rd_w(mem, program_count);

    uint32_t opcode, rd, funct3, rs1, rs2, funct7, funct12;
    int32_t  imm_110, imm_40, imm_3112;

    opcode   = extractBits(instruction, 6, 0);
    rd       = extractBits(instruction, 11, 7);
    funct3   = extractBits(instruction, 14, 12);
    rs1      = extractBits(instruction, 19, 15);
    rs2      = extractBits(instruction, 24, 20);
    funct7   = extractBits(instruction, 31, 25);
    funct12  = extractBits(instruction, 31, 20);
    imm_110  = sign_extend32(extractBits(instruction, 31, 20), 12);
    imm_40   = sign_extend32((extractBits(instruction, 11, 7) |
                            (extractBits(instruction, 31, 25) << 5)),
                             12);
    imm_3112 = sign_extend32(extractBits(instruction, 31, 12), 20);

    if (log_file) {
      fprintf(log_file, "PC: 0x%08x: Instruction:  0x%08x\n", program_count,
              instruction);
    }

    // check for R-type
    if (opcode == 0b0110011) {
      // ADD
      if (funct3 == 0b000 && funct7 == 0b0000000) {
        registers[rd] = registers[rs1] + registers[rs2];
      }
      // SUB
      else if (funct3 == 0b000 && funct7 == 0b0100000) {
        registers[rd] = registers[rs1] - registers[rs2];
      }
      // SLL
      else if (funct3 == 0b001 && funct7 == 0b0000000) {
        registers[rd] = registers[rs1] << registers[rs2];
      }
      // SLT
      else if (funct3 == 0b010 && funct7 == 0b0000000) {
        if (registers[rs1] < registers[rs2]) {
          registers[rd] = 1;
        } else {
          registers[rd] = 0;
        }
      }
      // SLTU
      else if (funct3 == 0b011 && funct7 == 0b0000000) {
        if ((uint32_t)registers[rs1] < (uint32_t)registers[rs2]) {
          registers[rd] = 1;
        } else {
          registers[rd] = 0;
        }
      }
      // XOR
      else if (funct3 == 0b100 && funct7 == 0b0000000) {
        registers[rd] = registers[rs1] ^ registers[rs2]; // turn back to this
      }
      // SRL
      else if (funct3 == 0b101 && funct7 == 0b0000000) {
        registers[rd] =
            (uint32_t)registers[rs1] >> (registers[rs2] & 0b000111111);
      }
      // SRA
      else if (funct3 == 0b101 && funct7 == 0b0100000) {
        registers[rd] = (int32_t)registers[rs1] >> registers[rs2];
      }
      // OR
      else if (funct3 == 0b110 && funct7 == 0b0000000) {
        registers[rd] = registers[rs1] | registers[rs2];
      }
      // AND
      else if (funct3 == 0b111 && funct7 == 0b0000000) {
        registers[rd] = registers[rs1] & registers[rs2];
      }

      // RV32M extension (e.g., MUL, DIV)
      else if (funct7 == 0b0000001) {
        // MUL
        if (funct3 == 0b000) {
          registers[rd] = registers[rs1] * registers[rs2];
        }
        // MULH
        else if (funct3 == 0b001) {
          int64_t result = (int64_t)(int32_t)registers[rs1] *
                           (int64_t)(int32_t)registers[rs2];
          registers[rd] = (int32_t)(result >> 32);
        }
        // MULHSU
        else if (funct3 == 0b010) {
          int64_t result =
              (int64_t)(int32_t)registers[rs1] * (uint64_t)registers[rs2];
          registers[rd] = (int32_t)(result >> 32);
        }
        // MULHU
        else if (funct3 == 0b011) {
          uint64_t result = (uint64_t)registers[rs1] * (uint64_t)registers[rs2];
          registers[rd]   = (uint32_t)(result >> 32);
        }
        // DIV
        else if (funct3 == 0b100) {
          if (registers[rs2] == 0) {
            // Division by zero
            registers[rd] = -1; // Quotient for signed division by zero
          } else if ((int32_t)registers[rs1] == INT32_MIN &&
                     (int32_t)registers[rs2] == -1) {
            // Signed division overflow
            registers[rd] = registers[rs1]; // Quotient equals the dividend
          } else {
            registers[rd] = (int32_t)registers[rs1] / (int32_t)registers[rs2];
          }
        }
        // DIVU
        else if (funct3 == 0b101) {
          if (registers[rs2] == 0) {
            // Division by zero
            registers[rd] =
                UINT32_MAX; // Quotient for unsigned division by zero
          } else {
            registers[rd] = (uint32_t)registers[rs1] / (uint32_t)registers[rs2];
          }
        }
        // REM
        else if (funct3 == 0b110) {
          if (registers[rs2] == 0) {
            // Division by zero
            registers[rd] = registers[rs1]; // Remainder equals the dividend
          } else if ((int32_t)registers[rs1] == INT32_MIN &&
                     (int32_t)registers[rs2] == -1) {
            // Signed division overflow
            registers[rd] = 0; // Remainder is zero
          } else {
            registers[rd] = (int32_t)registers[rs1] % (int32_t)registers[rs2];
          }
        }
        // REMU
        else if (funct3 == 0b111) {
          if (registers[rs2] == 0) {
            // Division by zero
            registers[rd] = registers[rs1]; // Remainder equals the dividend
          } else {
            registers[rd] = (uint32_t)registers[rs1] % (uint32_t)registers[rs2];
          }
        } else {
          fprintf(stderr,
                  "unknown R-type: (opcode=0xx%d, funct3=%d, funct7=%d)\n",
                  opcode, funct3, funct7);
          program_count += 4;
        }
      } else {
        fprintf(stderr, "unknown extension R-type\n");
      }
      program_count += 4;
      insns++;
    }

    // check for I-type
    // JALR
    else if (opcode == 0b1100111) {
      uint32_t target =
          (registers[rs1] + imm_110) & ~1; // Compute target address, clear LSB
      if (rd != 0) {                       // Do not write to x0
        registers[rd] = program_count + 4; // Store return address
      }
      program_count = target; // Jump to target
      insns++;
      continue; // Skip PC increment at the end of the loop

    } else if (opcode == 0b0000011) { // I-Type Load instructions
      uint32_t address = registers[rs1] + imm_110;

      // LB: Load Byte
      if (funct3 == 0b000) {
        registers[rd] = (int8_t)memory_rd_b(mem, address);
      }
      // LH: Load Halfword
      else if (funct3 == 0b001) {
        registers[rd] = (int16_t)memory_rd_h(mem, address);
      }
      // LW: Load Word
      else if (funct3 == 0b010) {
        registers[rd] = memory_rd_w(mem, address);
      }
      // LBU: Load Byte Unsigned
      else if (funct3 == 0b100) {
        registers[rd] = (uint8_t)memory_rd_b(mem, address);
      }
      // LHU: Load Halfword Unsigned
      else if (funct3 == 0b101) {
        registers[rd] = (uint16_t)memory_rd_h(mem, address);
      } else {
        fprintf(stderr,
                "ERROR: Unknown I-Type Load instruction (funct3=0x%x)\n",
                funct3);
      }

      // Increment program_count to move to the next instruction
      program_count += 4;
      insns++;
    }

    else if (opcode == 0b0010011) { // I-Type instructions
      // ADDI
      if (funct3 == 0b000) {
        registers[rd] = registers[rs1] + imm_110;
      }
      // SLTI
      else if (funct3 == 0b010) {
        registers[rd] = ((int32_t)registers[rs1] < imm_110) ? 1 : 0;
      }
      // SLTIU
      else if (funct3 == 0b011) {
        registers[rd] = ((uint32_t)registers[rs1] < (uint32_t)imm_110) ? 1 : 0;
      }
      // XORI
      else if (funct3 == 0b100) {
        registers[rd] = registers[rs1] ^ imm_110;
      }
      // ORI
      else if (funct3 == 0b110) {
        registers[rd] = registers[rs1] | imm_110;
      }
      // ANDI
      else if (funct3 == 0b111) {
        registers[rd] = registers[rs1] & imm_110;
      }
      // SLLI
      else if (funct3 == 0b001 && funct7 == 0b0000000) {
        registers[rd] = registers[rs1] << (imm_110 & 0x1F);
      }
      // SRLI / SRAI
      else if (funct3 == 0b101) {
        if (funct7 == 0b0000000) { // SRLI
          registers[rd] = (uint32_t)registers[rs1] >> (imm_110 & 0x1F);
        } else if (funct7 == 0b0100000) { // SRAI
          registers[rd] = (int32_t)registers[rs1] >> (imm_110 & 0x1F);
        } else {
          fprintf(stderr, "ERROR: Unknown I-Type instruction\n");
        }
      } else {
        fprintf(stderr, "ERROR: Unknown I-Type instruction\n");
      }
      // Increment program_count for valid or unknown instructions
      program_count += 4;
      insns++;
    }

    else if (opcode == 0b0100011) { // S-Type instructions
      uint32_t address = registers[rs1] + imm_40;

      // SB: Store Byte
      if (funct3 == 0b000) {
        memory_wr_b(mem, address, registers[rs2] & 0xFF);
      }
      // SH: Store Halfword
      else if (funct3 == 0b001) {
        memory_wr_h(mem, address, registers[rs2] & 0xFFFF);
      }
      // SW: Store Word
      else if (funct3 == 0b010) {
        memory_wr_w(mem, address, registers[rs2]);
      } else {
        fprintf(stderr, "ERROR: Unknown S-Type instruction (funct3=0x%x)\n",
                funct3);
      }

      // Increment program_count to move to the next instruction
      program_count += 4;
      insns++;
    }

    // check for U-type
    // LUI
    else if (opcode == 0b0110111) {
      if (rd != 0) {                    // Avoid writing to x0
        registers[rd] = imm_3112 << 12; // Immediate shifted left by 12 bits
      }
      program_count += 4; // Move to the next instruction
      insns++;

      // AUIPC
    } else if (opcode == 0b0010111) {
      if (rd != 0) { // Avoid writing to x0
        registers[rd] = program_count +
                        (imm_3112 << 12); // Immediate shifted left by 12 bits
      }
      program_count += 4; // Move to the next instruction
      insns++;
    }

    // check for J-type
    // JAL
    else if (opcode == 0b1101111) {
      /* if (rd != 0) {                       // Avoid writing to x0
        registers[rd] = program_count + 4; // Store return address
      }
      program_count += imm_3112; // Jump to target address
      // if not work maybe recalculate the jump target */

      if (rd != 0) {                       // Avoid writing to x0
        registers[rd] = program_count + 4; // Store return address
      }
      // Calculate the jump target
      int32_t immediate = ((instruction >> 31) & 1) << 20 |    // imm[20]
                          ((instruction >> 21) & 0x3FF) << 1 | // imm[10:1]
                          ((instruction >> 20) & 1) << 11 |    // imm[11]
                          ((instruction >> 12) & 0xFF) << 12;  // imm[19:12]
      immediate = sign_extend32(immediate, 21); // Sign-extend the immediate

      // Update program counter to target address
      program_count += immediate;
      insns++;
      continue; // Skip the default PC increment
    }

    // check for B-type
    else if (opcode == 0b1100011) {
      int32_t imm_b = (((instruction >> 31) & 1) << 12) |
                      (((instruction >> 25) & 0x3F) << 5) |
                      (((instruction >> 8) & 0xF) << 1) |
                      (((instruction >> 7) & 1) << 11);

      imm_b = sign_extend32(imm_b, 13);

      if (funct3 == 0b000) { // BEQ
        if (registers[rs1] == registers[rs2])
          program_count += imm_b;
        else
          program_count += 4;
      } else if (funct3 == 0b001) { // BNE
        if (registers[rs1] != registers[rs2])
          program_count += imm_b;
        else
          program_count += 4;
      } else if (funct3 == 0b100) { // BLT
        if ((int32_t)registers[rs1] < (int32_t)registers[rs2])
          program_count += imm_b;
        else
          program_count += 4;
      } else if (funct3 == 0b101) { // BGE
        if ((int32_t)registers[rs1] >= (int32_t)registers[rs2])
          program_count += imm_b;
        else
          program_count += 4;
      } else if (funct3 == 0b110) { // BLTU
        if ((uint32_t)registers[rs1] < (uint32_t)registers[rs2])
          program_count += imm_b;
        else
          program_count += 4;
      } else if (funct3 == 0b111) { // BGEU
        if ((uint32_t)registers[rs1] >= (uint32_t)registers[rs2])
          program_count += imm_b;
        else
          program_count += 4;
      } else {
        fprintf(stderr, "unknown B-type\n");
        program_count += 4;
      }
      insns++;
    }

    // system calls
    else if (opcode == 0b1110011 && funct3 == 0b000 &&
             funct12 == 0b000000000000) {
      // System call 1: Return getchar() in A0
      if (registers[17] == 1) {
        int input_char = getchar();
        if (input_char == EOF) {
          // No more input, force exit
          registers[17] = 93; // a7 = 93 means exit syscall
        } else {
          registers[10] = input_char;
        }

        // System call 2: Perform putchar(c), where c is in A0
      } else if (registers[17] == 2) {
        putchar((char)registers[10]);
        fflush(stdout); // Ensure immediate output

        // System call 3 or 93: Exit the simulation
      } else if (registers[17] == 3 || registers[17] == 93) {
        break;
      } else {
        fprintf(stderr, "Unknown system call: %u\n", registers[17]);
        program_count += 4;
        insns++;
      }
      program_count += 4;
      insns++;
    }
  }
  return (struct Stat){.insns = insns};
}
