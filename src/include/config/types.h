/**
 * @file types.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief types definition
 * @version 0.1
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iomanip>

namespace jasonfxz {

using AddrType = uint32_t; // 32-bit address
using DataType = uint32_t; // 32-bit data
using WordType = uint32_t; // 32-bit word
using HalfType = uint16_t; // 16-bit half word
using ByteType = uint8_t;  // 8-bit byte
using ReturnType = uint8_t; // return type

enum class OpClass {
    OTHER,
    BRANCH,
    LOAD,
    STORE,
    ARITHI,
    ARITHR,
};


// Opcode Type
enum OpType {
    NONE = 0,
    // OTHER
    LUI,   // Load Upper Immediate               0b011'0111    U-type
    AUIPC, // Add Upper Immediate to PC          0b001'0111    U-type
    JAL,   // Jump And Link                      0b110'1111    J-type
    JALR,  // Jump And Link Register             0b110'0111    I-type

    // BRANCH
    BEQ,   // Branch Equal                       0b110'0011    B-type
    BNE,   // Branch Not Equal                   0b110'0011    B-type
    BLT,   // Branch Less Than                   0b110'0011    B-type
    BGE,   // Branch Greater Than Equal          0b110'0011    B-type
    BLTU,  // Branch Less Than Unsigned          0b110'0011    B-type
    BGEU,  // Branch Greater Than Equal Unsigned 0b110'0011    B-type

    // LOAD
    LB,    // Load Byte                          0b000'0011    I-type
    LH,    // Load Halfword                      0b000'0011    I-type
    LW,    // Load Word                          0b000'0011    I-type
    LBU,   // Load Byte Unsigned                 0b000'0011    I-type
    LHU,   // Load Halfword Unsigned             0b000'0011    I-type

    // STORE
    SB,    // Store Byte                         0b010'0011    S-type
    SH,    // Store Halfword                     0b010'0011    S-type
    SW,    // Store Word                         0b010'0011    S-type

    // ARITHI  ARITHMETIC IMMEDIATE
    ADDI,  // Add Immediate                      0b001'0011    I-type
    SLTI,  // Set Less Than Immediate            0b001'0011    I-type
    SLTIU, // Set Less Than Immediate Unsigned   0b001'0011    I-type
    XORI,  // XOR Immediate                      0b001'0011    I-type
    ORI,   // OR Immediate                       0b001'0011    I-type
    ANDI,  // AND Immediate                      0b001'0011    I-type
    SLLI,  // Shift Left Logical Immediate       0b001'0011    I-type
    SRLI,  // Shift Right Logical Immediate      0b001'0011    I-type
    SRAI,  // Shift Right Arithmetic Immediate   0b001'0011    I-type

    // ARITHR  ARITHMETIC REGISTER
    ADD,   // Add                                0b011'0011    R-type
    SUB,   // Subtract                           0b011'0011    R-type
    SLL,   // Shift Left Logical                 0b011'0011    R-type
    SLT,   // Set Less Than                      0b011'0011    R-type
    SLTU,  // Set Less Than Unsigned             0b011'0011    R-type
    XOR,   // XOR                                0b011'0011    R-type
    SRL,   // Shift Right Logical                0b011'0011    R-type
    SRA,   // Shift Right Arithmetic             0b011'0011    R-type
    OR,    // OR                                 0b011'0011    R-type
    AND,   // AND                                0b011'0011    R-type



};


// name of reg
enum reName {
    zero = 0,
    ra = 1,
    sp = 2,
    gp = 3,
    tp = 4,
    t0 = 5,
    t1 = 6,
    t2 = 7,
    s0 = 8,
    s1 = 9,
    a0 = 10,
    a1 = 11,
    a2 = 12,
    a3 = 13,
    a4 = 14,
    a5 = 15,
    a6 = 16,
    a7 = 17,
    s2 = 18,
    s3 = 19,
    s4 = 20,
    s5 = 21,
    s6 = 22,
    s7 = 23,
    s8 = 24,
    s9 = 25,
    s10 = 26,
    s11 = 27,
    t3 = 28,
    t4 = 29,
    t5 = 30,
    t6 = 31,
};

std::string OpcodeToStr(OpType opt);

// Instruction Type
struct InsType {
    friend class Decoder;
    friend class InstructionUnit;
    friend class Simulator;

  private:
    int opcode;
    DataType ir;
    int funct3, funct7;
  public:
    OpType opt;
    OpClass opc;
    int rd{-1}, rs1{-1}, rs2{-1}, imm;
    AddrType ins_addr;
    friend std::ostream &operator<<(std::ostream &os, const InsType &ins) {
        os << "IR: " << std::setw(8) << std::setfill('0') << std::hex << ins.ir
           << std::dec << std::setfill(' ')
           << " OP: " << std::setw(5) << OpcodeToStr(ins.opt)
           << " RD: " << std::setw(3) << ins.rd << " RS1: " << std::setw(3) << ins.rs1
           << " RS2: " << std::setw(3) << ins.rs2 << " IMM: " << std::setw(10) << ins.imm;
        return os;
    }


    void Print(std::ostream &os) {
        os << "PC: " << std::setw(4) << std::setfill('0') << std::hex << this->ins_addr
                  << " IR: " << std::setw(8) << std::setfill('0') << this->ir << std::endl;
        os << std::dec;
        switch (this->opt) {
        case ADD: case SUB: case AND: case OR: case XOR: case SLL: case SRL: case SRA: case SLT: case SLTU:
            os << OpcodeToStr(this->opt) << " x" << this->rd << ", x" << this->rs1 << ", x" << this->rs2 << std::endl; break;
        case ADDI: case ANDI: case ORI: case XORI: case SLTI: case SLTIU:
            os << OpcodeToStr(this->opt) << " x" << this->rd << ", x" << this->rs1 << ", " << this->imm << std::endl; break;
        case SLLI: case SRLI: case SRAI:
            os << OpcodeToStr(this->opt) << " x" << this->rd << ", x" << this->rs1 << ", " <<
                      (uint32_t)this->imm << std::endl;
            break;
        case LB: case LBU: case LH: case LHU: case LW:
            os << OpcodeToStr(this->opt) << " x" << this->rd << ", " << this->imm << "(x" << this->rs1 << ")" << std::endl;
            break;
        case SB: case SH: case SW:
            os << OpcodeToStr(this->opt) << " x" << this->rs1 << ", " << this->imm << "(x" << this->rs2 << ")" << std::endl;
            break;
        case BEQ: case BNE: case BLT: case BLTU: case BGE: case BGEU:
            os << OpcodeToStr(this->opt) << " x" << this->rs1 << ", x" << this->rs2 << "  <" << this->imm << ">" <<
                      std::endl; break;
        case JAL:
            os << OpcodeToStr(this->opt) << " x" << this->rd << "  <" << this->imm << ">" << std::endl; break;
        case JALR:
            os << OpcodeToStr(this->opt) << " x" << this->rd << ", x" << this->rs1 << "  <" << this->imm << ">" << std::endl;
            break;
        case AUIPC:
            os << OpcodeToStr(this->opt) << " x" << this->rd << "  u" << this->imm << std::endl; break;
        case LUI:
            os << OpcodeToStr(this->opt) << " x" << this->rd << "  u" << this->imm  << std::endl; break;
        case NONE:
            throw "Invalid Instruction";
            break;
        };
    }
};




} // namespace jasonfxz


#endif // TYPES_H