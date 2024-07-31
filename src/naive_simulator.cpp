/**
 * @file naive_simulator.cpp
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief A naive simulator for RISC-V RV32I
 * @version 0.1
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "include/config/types.h"
#include "include/config/constant.h"
#include "include/utils/utils.h"
#include "include/naive_simulator.h"
#include <iostream>
#include <iomanip>
#include <cstring>

namespace jasonfxz {


void NSimulator::ExeADD(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] + reg[rs2];
}

void NSimulator::ExeSUB(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] - reg[rs2];
}

void NSimulator::ExeAND(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] & reg[rs2];
}

void NSimulator::ExeOR(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] | reg[rs2];
}

void NSimulator::ExeXOR(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] ^ reg[rs2];
}

void NSimulator::ExeSLL(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] << reg[rs2];
}

void NSimulator::ExeSRL(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] >> reg[rs2];
}

void NSimulator::ExeSRA(int rd, int rs1, int rs2) {
    if (reg[rs1] & (1 << 31)) {
        reg[rd] = (reg[rs1] >> reg[rs2]) | (0xFFFFFFFF << (32 - reg[rs2]));
    } else {
        reg[rd] = reg[rs1] >> reg[rs2];
    }
}

void NSimulator::ExeSLT(int rd, int rs1, int rs2) {
    reg[rd] = (int32_t)reg[rs1] < (int32_t)reg[rs2] ? 1 : 0;
}

void NSimulator::ExeSLTU(int rd, int rs1, int rs2) {
    reg[rd] = reg[rs1] < reg[rs2] ? 1 : 0;
}

void NSimulator::ExeADDI(int rd, int rs1, int imm) {
    reg[rd] = reg[rs1] + imm;
}

void NSimulator::ExeANDI(int rd, int rs1, int imm) {
    reg[rd] = reg[rs1] & imm;
}

void NSimulator::ExeORI(int rd, int rs1, int imm) {
    reg[rd] = reg[rs1] | imm;
}

void NSimulator::ExeXORI(int rd, int rs1, int imm) {
    reg[rd] = reg[rs1] ^ imm;
}

void NSimulator::ExeSLLI(int rd, int rs1, int imm) {
    reg[rd] = reg[rs1] << imm;
}

void NSimulator::ExeSRLI(int rd, int rs1, int imm) {
    reg[rd] = reg[rs1] >> imm;
}

void NSimulator::ExeSRAI(int rd, int rs1, int imm) {
    if (reg[rs1] & (1 << 31)) {
        reg[rd] = (reg[rs1] >> imm) | (0xFFFFFFFF << (32 - imm));
    } else {
        reg[rd] = reg[rs1] >> imm;
    }
}

void NSimulator::ExeSLTI(int rd, int rs1, int imm) {
    reg[rd] = (int32_t)reg[rs1] < imm ? 1 : 0;
}

void NSimulator::ExeSLTUI(int rd, int rs1, int imm) {
    reg[rd] = reg[rs1] < (uint32_t)imm ? 1 : 0;
}

void NSimulator::ExeLB(int rd, int rs1, int imm) {
    reg[rd] = SEXT(mem[reg[rs1] + imm]);
}

void NSimulator::ExeLBU(int rd, int rs1, int imm) {
    reg[rd] = ZEXT(mem[reg[rs1] + imm]);
}

void NSimulator::ExeLH(int rd, int rs1, int imm) {
    reg[rd] = SEXT(Concat(mem[reg[rs1] + imm], mem[reg[rs1] + imm + 1]));
}

void NSimulator::ExeLHU(int rd, int rs1, int imm) {
    reg[rd] = ZEXT(Concat(mem[reg[rs1] + imm], mem[reg[rs1] + imm + 1]));
}

void NSimulator::ExeLW(int rd, int rs1, int imm) {
    reg[rd] = Concat(mem[reg[rs1] + imm], mem[reg[rs1] + imm + 1], mem[reg[rs1] + imm + 2], mem[reg[rs1] + imm + 3]);
}

void NSimulator::ExeSB(int rs1, int rs2, int imm) {
    mem[reg[rs1] + imm] = GetByte(reg[rs2], 0);
}

void NSimulator::ExeSH(int rs1, int rs2, int imm) {
    mem[reg[rs1] + imm] = GetByte(reg[rs2], 0);
    mem[reg[rs1] + imm + 1] = GetByte(reg[rs2], 1);
}

void NSimulator::ExeSW(int rs1, int rs2, int imm) {
    mem[reg[rs1] + imm] = GetByte(reg[rs2], 0);
    mem[reg[rs1] + imm + 1] = GetByte(reg[rs2], 1);
    mem[reg[rs1] + imm + 2] = GetByte(reg[rs2], 2);
    mem[reg[rs1] + imm + 3] = GetByte(reg[rs2], 3);
}

void NSimulator::ExeBEQ(int rs1, int rs2, int off) {
    if (reg[rs1] == reg[rs2]) {
        pc += off;
        pc_sel = 1;
    }
}

void NSimulator::ExeBGE(int rs1, int rs2, int off) {
    if ((int32_t)reg[rs1] >= (int32_t)reg[rs2]) {
        pc += off;
        pc_sel = 1;
    }
}

void NSimulator::ExeBGEU(int rs1, int rs2, int off) {
    if (reg[rs1] >= reg[rs2]) {
        pc += off;
        pc_sel = 1;
    }
}

void NSimulator::ExeBLT(int rs1, int rs2, int off) {
    if ((int32_t)reg[rs1] < (int32_t)reg[rs2]) {
        pc += off;
        pc_sel = 1;
    }
}

void NSimulator::ExeBLTU(int rs1, int rs2, int off) {
    if (reg[rs1] < reg[rs2]) {
        pc += off;
        pc_sel = 1;
    }
}

void NSimulator::ExeBNE(int rs1, int rs2, int off) {
    if (reg[rs1] != reg[rs2]) {
        pc += off;
        pc_sel = 1;
    }
}

void NSimulator::ExeJAL(int rd, int off) {
    reg[rd] = pc + 4;
    pc += off;
    pc_sel = 1;
}

void NSimulator::ExeJALR(int rd, int rs1, int off) {
    reg[rd] = pc + 4;
    pc = reg[rs1] + off;
    pc_sel = 1;
}

void NSimulator::ExeAUIPC(int rd, int immu) {
    reg[rd] = pc + (immu << 12);
}

void NSimulator::ExeLUI(int rd, int immu) {
    reg[rd] = immu << 12;
}

inline int HexToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return c - 'A' + 10;
    }
}

void NSimulator::Init(std::istream &is) {
    memset(mem, 0, sizeof(mem));
    memset(reg, 0, sizeof(reg));
    // Read the memory from stdin
    char input_str[10];
    AddrType cur_addr = 0x0;
    while (is >> input_str) {
        if (input_str[0] == '@') {
            cur_addr = 0x0;
            for (int i = 1; i <= 8; i++) {
                cur_addr = (cur_addr << 4) + HexToInt(input_str[i]);
            }
        } else {
            ByteType current_data = (HexToInt(input_str[0]) << 4) + HexToInt(input_str[1]);
            mem[cur_addr++] = current_data;
        }
    }
}

void NSimulator::FetchDecode() {
    // Fetch the instruction from memory
    ins.ir = (Concat(mem[pc], mem[pc + 1], mem[pc + 2], mem[pc + 3]));
    // Decode the instruction
    decoder.Decode(ins);
#ifdef DEBUG
    if (enable_debug) {
        std::cerr << "NAIVE PC: " << std::setw(4) << std::setfill('0') << std::hex << pc
                  << " IR: " << std::setw(8) << std::setfill('0') << ins.ir << std::endl;
        std::cerr << std::dec;
        switch (ins.opt) {
        case ADD: case SUB: case AND: case OR: case XOR: case SLL: case SRL: case SRA: case SLT: case SLTU:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << ", x" << ins.rs1 << ", x" << ins.rs2 << std::endl; break;
        case ADDI: case ANDI: case ORI: case XORI: case SLTI: case SLTIU:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << ", x" << ins.rs1 << ", " << ins.imm << std::endl; break;
        case SLLI: case SRLI: case SRAI:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << ", x" << ins.rs1 << ", " << (uint32_t)ins.imm << std::endl;
            break;
        case LB: case LBU: case LH: case LHU: case LW:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << ", " << ins.imm << "(x" << ins.rs1 << ")" << std::endl; break;
        case SB: case SH: case SW:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rs1 << ", " << ins.imm << "(x" << ins.rs2 << ")" << std::endl; break;
        case BEQ: case BNE: case BLT: case BLTU: case BGE: case BGEU:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rs1 << ", x" << ins.rs2 << "  <" << ins.imm << ">" << std::endl; break;
        case JAL:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << "  <" << ins.imm << ">" << std::endl; break;
        case JALR:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << ", x" << ins.rs1 << "  <" << ins.imm << ">" << std::endl; break;
        case AUIPC:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << "  u" << ins.imm << std::endl; break;
        case LUI:
            std::cerr << OpcodeToStr(ins.opt) << " x" << ins.rd << "  u" << ins.imm  << std::endl; break;
        case NONE:
            throw "Invalid Instruction";
            break;
        };
    }
#endif
}

void nDecoder::Decode(Instruction &ins) {
    ins.opcode = GetByte(ins.ir, 0) & 0x7F;  /// [6:0]
    switch (ins.opcode) {
    case 0b011'0111:
    case 0b001'0111:
        DecodeU(ins); break;
    case 0b110'1111:
        DecodeJ(ins); break;
    case 0b110'0111:
    case 0b000'0011:
    case 0b001'0011:
        DecodeI(ins); break;
    case 0b110'0011:
        DecodeB(ins); break;
    case 0b010'0011:
        DecodeS(ins); break;
    case 0b011'0011:
        DecodeR(ins); break;
    };
}

void nDecoder::GetFunct3(Instruction &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.funct3 = (GetByte(ins.ir, 1) & 0b0111'0000U) >> 4; /// [14:12]
}
void nDecoder::GetFunct7(Instruction &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.funct7 = (GetByte(ins.ir, 3) & 0b1111'1110U) >> 1; /// [31:25]
}
void nDecoder::GetRd(Instruction &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.rd = (ins.ir & 0b1111'1000'0000U) >> 7; /// [11:7]
}
void nDecoder::GetRs1(Instruction &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.rs1 = (ins.ir & 0b1111'1000'0000'0000'0000U) >> 15; /// [19:15]
}

void nDecoder::GetRs2(Instruction &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.rs2 = (ins.ir & 0b0001'1111'0000'0000'0000'0000'0000U) >> 20; /// [24:20]
}


void nDecoder::DecodeR(Instruction &ins) {
    GetFunct7(ins);
    GetRs2(ins);
    GetRs1(ins);
    GetFunct3(ins);
    GetRd(ins);
    switch (ins.funct3) {
    case 0b000:
        if (ins.funct7 == 0b000'0000) ins.opt = ADD;
        if (ins.funct7 == 0b010'0000) ins.opt = SUB;
        break;
    case 0b111:
        if (ins.funct7 == 0b000'0000) ins.opt = AND;
        break;
    case 0b110:
        if (ins.funct7 == 0b000'0000) ins.opt = OR;
        break;
    case 0b100:
        if (ins.funct7 == 0b000'0000) ins.opt = XOR;
        break;
    case 0b001:
        if (ins.funct7 == 0b000'0000) ins.opt = SLL;
        break;
    case 0b101:
        if (ins.funct7 == 0b000'0000) ins.opt = SRL;
        if (ins.funct7 == 0b010'0000) ins.opt = SRA;
        break;
    case 0b010:
        if (ins.funct7 == 0b000'0000) ins.opt = SLT;
        break;
    case 0b011:
        if (ins.funct7 == 0b000'0000) ins.opt = SLTU;
        break;
    }
}

void nDecoder::DecodeI(Instruction &ins) {
    GetRs1(ins);
    GetFunct3(ins);
    GetRd(ins);
    GetFunct7(ins);
    // 31 - 20 Sign-Extend
    ins.imm = SEXT(ins.ir >> 20, 12);
    switch (ins.opcode) {
    case 0b001'0011:
        switch (ins.funct3) {
        case 0b000:
            ins.opt = ADDI; break;
        case 0b111:
            ins.opt = ANDI; break;
        case 0b110:
            ins.opt = ORI; break;
        case 0b100:
            ins.opt = XORI; break;
        case 0b001:
            // 24 ~ 20
            ins.imm = ZEXT(ins.ir >> 20, 5);
            if (ins.funct7 == 0b000'0000) ins.opt = SLLI;
            break;
        case 0b101:
            // 24 ~ 20
            ins.imm = ZEXT(ins.ir >> 20, 5);
            if (ins.funct7 == 0b000'0000) ins.opt = SRLI;
            if (ins.funct7 == 0b010'0000) ins.opt = SRAI;
            break;
        case 0b010:
            ins.opt = SLTI; break;
        case 0b011:
            ins.opt = SLTIU; break;
        };
        break;
    case 0b000'0011:
        switch (ins.funct3) {
        case 0b000:
            ins.opt = LB; break;
        case 0b001:
            ins.opt = LH; break;
        case 0b010:
            ins.opt = LW; break;
        case 0b100:
            ins.opt = LBU; break;
        case 0b101:
            ins.opt = LHU; break;
        };
        break;
    case 0b110'0111:
        if (ins.funct3 == 0b000) ins.opt = JALR;
        break;
    };
}

void nDecoder::DecodeS(Instruction &ins) {
    GetRs1(ins);
    GetRs2(ins);
    GetFunct3(ins);
    ins.imm = SEXT(Concat((ins.ir >> 7) & 0b1'1111, 5, (ins.ir >> 25)), 12);
    if (ins.opcode == 0b010'0011) {
        switch (ins.funct3) {
        case 0b000:
            ins.opt = SB; break;
        case 0b001:
            ins.opt = SH; break;
        case 0b010:
            ins.opt = SW; break;
        };
    }
}

void nDecoder::DecodeB(Instruction &ins) {
    GetRs1(ins);
    GetRs2(ins);
    GetFunct3(ins);
    // 11:8 => 4:1  7=>11  30:25 => 10:5 31 => 12
    ins.imm = SEXT(((ins.ir >> 7) & 0b1'1110)
                   | ((ins.ir << 4) & 0b1000'0000'0000)
                   | ((ins.ir >> 20) & 0b0111'1110'0000)
                   | ((ins.ir >> 19) & 0b0001'0000'0000'0000), 13);
    if (ins.opcode == 0b110'0011) {
        switch (ins.funct3) {
        case 0b000:
            ins.opt = BEQ; break;
        case 0b001:
            ins.opt = BNE; break;
        case 0b100:
            ins.opt = BLT; break;
        case 0b110:
            ins.opt = BLTU; break;
        case 0b101:
            ins.opt = BGE; break;
        case 0b111:
            ins.opt = BGEU; break;
        };
    }
}

void nDecoder::DecodeU(Instruction &ins) {
    GetRd(ins);
    //  31:12 => 31:12
    ins.imm = (ins.ir & 0xFFFFF000) >> 12;
    switch (ins.opcode) {
    case 0b011'0111:
        ins.opt = LUI; break;
    case 0b001'0111:
        ins.opt = AUIPC; break;
    }
}

void nDecoder::DecodeJ(Instruction &ins) {
    GetRd(ins);
    // 31=>20  30:21=>10:1  20=>11  19:12=>19:12
    ins.imm = SEXT(((ins.ir >> 11) & 0b1'0000'0000'0000'0000'0000)
                   | ((ins.ir >> 20) & 0b0111'1111'1110)
                   | ((ins.ir >> 9) & 0b1000'0000'0000)
                   | ((ins.ir) & 0b1111'1111'0000'0000'0000), 21);
    if (ins.opcode == 0b110'1111) {
        ins.opt = JAL;
    }
}


void NSimulator::Execute() {
    switch (ins.opt) {
    case ADD:
        ExeADD(ins.rd, ins.rs1, ins.rs2); break;
    case SUB:
        ExeSUB(ins.rd, ins.rs1, ins.rs2); break;
    case AND:
        ExeAND(ins.rd, ins.rs1, ins.rs2); break;
    case OR:
        ExeOR(ins.rd, ins.rs1, ins.rs2); break;
    case XOR:
        ExeXOR(ins.rd, ins.rs1, ins.rs2); break;
    case SLL:
        ExeSLL(ins.rd, ins.rs1, ins.rs2); break;
    case SRL:
        ExeSRL(ins.rd, ins.rs1, ins.rs2); break;
    case SRA:
        ExeSRA(ins.rd, ins.rs1, ins.rs2); break;
    case SLT:
        ExeSLT(ins.rd, ins.rs1, ins.rs2); break;
    case SLTU:
        ExeSLTU(ins.rd, ins.rs1, ins.rs2); break;
    case ADDI:
        ExeADDI(ins.rd, ins.rs1, ins.imm); break;
    case ANDI:
        ExeANDI(ins.rd, ins.rs1, ins.imm); break;
    case ORI:
        ExeORI(ins.rd, ins.rs1, ins.imm); break;
    case XORI:
        ExeXORI(ins.rd, ins.rs1, ins.imm); break;
    case SLLI:
        ExeSLLI(ins.rd, ins.rs1, ins.imm); break;
    case SRLI:
        ExeSRLI(ins.rd, ins.rs1, ins.imm); break;
    case SRAI:
        ExeSRAI(ins.rd, ins.rs1, ins.imm); break;
    case SLTI:
        ExeSLTI(ins.rd, ins.rs1, ins.imm); break;
    case SLTIU:
        ExeSLTUI(ins.rd, ins.rs1, ins.imm); break;
    case LB:
        ExeLB(ins.rd, ins.rs1, ins.imm); break;
    case LBU:
        ExeLBU(ins.rd, ins.rs1, ins.imm); break;
    case LH:
        ExeLH(ins.rd, ins.rs1, ins.imm); break;
    case LHU:
        ExeLHU(ins.rd, ins.rs1, ins.imm); break;
    case LW:
        ExeLW(ins.rd, ins.rs1, ins.imm); break;
    case SB:
        ExeSB(ins.rs1, ins.rs2, ins.imm); break;
    case SH:
        ExeSH(ins.rs1, ins.rs2, ins.imm); break;
    case SW:
        ExeSW(ins.rs1, ins.rs2, ins.imm); break;
    case BEQ:
        ExeBEQ(ins.rs1, ins.rs2, ins.imm); break;
    case BNE:
        ExeBNE(ins.rs1, ins.rs2, ins.imm); break;
    case BLT:
        ExeBLT(ins.rs1, ins.rs2, ins.imm); break;
    case BLTU:
        ExeBLTU(ins.rs1, ins.rs2, ins.imm); break;
    case BGE:
        ExeBGE(ins.rs1, ins.rs2, ins.imm); break;
    case BGEU:
        ExeBGEU(ins.rs1, ins.rs2, ins.imm); break;
    case JAL:
        ExeJAL(ins.rd, ins.imm); break;
    case JALR:
        ExeJALR(ins.rd, ins.rs1, ins.imm); break;
    case AUIPC:
        ExeAUIPC(ins.rd, ins.imm); break;
    case LUI:
        ExeLUI(ins.rd, ins.imm); break;
    case NONE:
        throw "Invalid Instruction";
        break;
    };
}

void NSimulator::PrintRegHelp() {
    std::cerr << "+**************************************************************************+" << std::endl;
    std::cerr << "+  #   + Name + Description           +  #   + Name + Description          +" << std::endl;
    std::cerr << "+******+******+***********************+******+******+**********************+" << std::endl;
    std::cerr << "+ x0   + zero + Constant 0            + x16  + a6   + Args                 +" << std::endl;
    std::cerr << "+ x1   + ra   + Return Address        + x17  + a7   +                      +" << std::endl;
    std::cerr << "+ x2   + sp   + Stack Pointer         + x18  + s2   + Saved Registers      +" << std::endl;
    std::cerr << "+ x3   + gp   + Global Pointer        + x19  + s3   +                      +" << std::endl;
    std::cerr << "+ x4   + tp   + Thread Pointer        + x20  + s4   +                      +" << std::endl;
    std::cerr << "+ x5   + t0   + Temporary Registers   + x21  + s5   +                      +" << std::endl;
    std::cerr << "+ x6   + t1   +                       + x22  + s6   +                      +" << std::endl;
    std::cerr << "+ x7   + t2   +                       + x23  + s7   +                      +" << std::endl;
    std::cerr << "+ x8   + s0   + Saved Registers       + x24  + s8   +                      +" << std::endl;
    std::cerr << "+ x9   + s1   +                       + x25  + s9   +                      +" << std::endl;
    std::cerr << "+ x10  + a0   + Function Arguments or + x26  + s10  +                      +" << std::endl;
    std::cerr << "+ x11  + a1   +  Return Values        + x27  + s11  +                      +" << std::endl;
    std::cerr << "+ x12  + a2   + Function Arguments    + x28  + t3   + Temporaries          +" << std::endl;
    std::cerr << "+ x13  + a3   +                       + x29  + t4   +                      +" << std::endl;
    std::cerr << "+ x14  + a4   +                       + x30  + t5   +                      +" << std::endl;
    std::cerr << "+ x15  + a5   +                       + x31  + t6   +                      +" << std::endl;
    std::cerr << "+**************************************************************************+" << std::endl;
}

void NSimulator::PrintReg() {
    std::cerr << "Naive Register File:" << std::endl;
    std::cerr << "+-----+----------+-------------+-----+----------+-------------+" << std::endl;
    std::cerr << "| Reg |      Hex |         Dec | Reg |      Hex |         Dec |" << std::endl;
    std::cerr << "+-----+----------+-------------+-----+----------+-------------+" << std::endl;
    for (int i = 0; i < REG_FILE_SIZE / 2; ++i) {
        std::cerr << "| x" << std::setfill(' ') << std::left << std::setw(2) << i << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << reg[i]
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << reg[i] << " | ";
        int j = i | REG_FILE_SIZE / 2;
        std::cerr << "x" <<  std::left << std::setw(2) << j << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << reg[j]
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << reg[j] << " |" << std::endl;
    }
    std::cerr << "+-----+----------+-------------+-----+----------+-------------+" << std::endl;
}

void NSimulator::PrintMem(AddrType addr, int len) {
    std::cerr << "Naive Memory:" << std::endl;
    std::cerr << "+--------+----------+-------------+" << std::endl;
    std::cerr << "|  Addr  |      Hex |         Dec |" << std::endl;
    std::cerr << "+--------+----------+-------------+" << std::endl;
    for (int i = 0; i < len; ++i) {
        std::cerr << "| " << std::setfill(' ') << std::left << std::setw(6) << std::dec << addr + i << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << (int)mem[addr + i]
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << (int)mem[addr + i] << " |" << std::endl;
    }
    std::cerr << "+--------+----------+-------------+" << std::endl;
}

bool NSimulator::Step(DebugRecord &record) {
    FetchDecode();
    record.pc = pc;
    record.ir = ins.ir;
    if (ins.ir == 0x0ff00513) return false; // terminate
    pc_sel = 0;
    Execute();
    for (int i = 0; i < REG_FILE_SIZE; ++i) {
        record.reg[i] = reg[i];
    }
    reg[zero] = 0;
    if (!pc_sel) pc += 4;
#ifdef DEBUG
    if (enable_debug)
        PrintReg();
#endif
    return true;
}

ReturnType NSimulator::Run() {
#ifdef DEBUG
    if (enable_debug)
        PrintRegHelp();
#endif
    DebugRecord record;
    while (Step(record));
    return reg[a0];
}



} // namespace jasonfxz



