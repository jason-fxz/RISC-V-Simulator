/**
 * @file naive_simulator.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief a naive RISC-V simulator
 * @version 0.1
 * @date 2024-07-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef NAIVE_SIMULATOR_H
#define NAIVE_SIMULATOR_H

#include "config/types.h"
#include "config/constant.h"
#include "units/memory_unit.h"


namespace jasonfxz {

struct Instruction {
    DataType ir;
    Optype opt;
    int opcode;
    int rd, rs1, rs2, imm;
    int funct3, funct7;
};

class Decoder {
  private:
    void DecodeR(Instruction &ins);
    void DecodeI(Instruction &ins);
    void DecodeS(Instruction &ins);
    void DecodeB(Instruction &ins);
    void DecodeU(Instruction &ins);
    void DecodeJ(Instruction &ins);

    void GetFunct3(Instruction &ins);
    void GetFunct7(Instruction &ins);
    void GetRd(Instruction &ins);
    void GetRs1(Instruction &ins);
    void GetRs2(Instruction &ins);

  public:
    void Decode(Instruction &ins);

};


class Simulator {
  private:
    AddrType pc;
    Instruction ins;
    bool pc_sel;

    DataType reg[REG_FILE_SIZE];  // registers
    ByteType mem[MAX_RAM_SIZE];   // memory

    Decoder decoder;

  private:
    void FetchDecode();
    void Execute();

  private: // Execute:

    // Arithmetic
    void ExeADD(int rd, int rs1, int rs2); // rd = rs1 + rs2
    void ExeSUB(int rd, int rs1, int rs2); // rd = rs1 - rs2
    void ExeAND(int rd, int rs1, int rs2); // rd = rs1 & rs2
    void ExeOR(int rd, int rs1, int rs2);  // rd = rs1 | rs2
    void ExeXOR(int rd, int rs1, int rs2); // rd = rs1 ^ rs2
    void ExeSLL(int rd, int rs1, int rs2); // rd = rs1 << rs2
    void ExeSRL(int rd, int rs1, int rs2); // rd = rs1 >> rs2 (Zero-extend)
    void ExeSRA(int rd, int rs1, int rs2); // rd = rs1 >> rs2 (Sign-extend)
    void ExeSLT(int rd, int rs1, int rs2); // rd = rs1 < rs2 ? 1 : 0 (signed)
    void ExeSLTU(int rd, int rs1, int rs2); // rd = rs1 < rs2 ? 1 : 0 (unsigned)

    void ExeADDI(int rd, int rs1, int imm); // rd = rs1 + imm
    void ExeANDI(int rd, int rs1, int imm); // rd = rs1 & imm
    void ExeORI(int rd, int rs1, int imm);  // rd = rs1 | imm
    void ExeXORI(int rd, int rs1, int imm); // rd = rs1 ^ imm
    void ExeSLLI(int rd, int rs1, int imm); // rd = rs1 << imm
    void ExeSRLI(int rd, int rs1, int imm); // rd = rs1 >> imm (Zero-extend)
    void ExeSRAI(int rd, int rs1, int imm); // rd = rs1 >> imm (Sign-extend)
    void ExeSLTI(int rd, int rs1, int imm); // rd = rs1 < imm ? 1 : 0 (signed)
    void ExeSLTUI(int rd, int rs1, int imm); // rd = rs1 < imm ? 1 : 0 (unsigned)

    // Memory
    void ExeLB(int rd, int rs1, int imm); // rd = mem[rs1 + imm] 1bit (signed)
    void ExeLBU(int rd, int rs1, int imm); // rd = mem[rs1 + imm] 1bit (unsigned)
    void ExeLH(int rd, int rs1, int imm); // rd = mem[rs1 + imm] 2bit (signed)
    void ExeLHU(int rd, int rs1, int imm); // rd = mem[rs1 + imm] 2bit (unsigned)
    void ExeLW(int rd, int rs1, int imm); // rd = mem[rs1 + imm] 4bit

    void ExeSB(int rs1, int rs2, int imm); // mem[rs1 + imm] = rs2 1bit
    void ExeSH(int rs1, int rs2, int imm); // mem[rs1 + imm] = rs2 2bit
    void ExeSW(int rs1, int rs2, int imm); // mem[rs1 + imm] = rs2 4bit

    // Control
    void ExeBEQ(int rs1, int rs2, int off); // if rs1 == rs2 then pc += off
    void ExeBGE(int rs1, int rs2, int off); // if rs1 >= rs2 (signed) then pc += off
    void ExeBGEU(int rs1, int rs2, int off); // if rs1 >= rs2 (unsigned) then pc += off
    void ExeBLT(int rs1, int rs2, int off); // if rs1 < rs2 (signed) then pc += off
    void ExeBLTU(int rs1, int rs2, int off); // if rs1 < rs2 (unsigned) then pc += off
    void ExeBNE(int rs1, int rs2, int off); // if rs1 != rs2 then pc += off
    void ExeJAL(int rd, int off); // rd = pc + 4, pc += off
    void ExeJALR(int rd, int rs1, int off); // rd = pc + 4, pc = rs1 + off

    // Other
    void ExeAUIPC(int rd, int immu); // rd = pc + (immu << 12)
    void ExeLUI(int rd, int immu); // rd = (immu << 12)


  public:
    void Init();
    bool Step();
    ReturnType Run();
    void PrintReg();
    void PrintRegHelp();
    void PrintMem(AddrType addr, int len);
    void Debug();

};

    
} // namespace jasonfxz



#endif