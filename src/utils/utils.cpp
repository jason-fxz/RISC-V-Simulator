#include "../include/utils/utils.h"
#include "config/types.h"



namespace jasonfxz {

// DataType ZEXT(DataType data, int len, int to) {
//     return data & ((1 << to) - 1);
// }
// DataType SEXT(DataType data, int len, int to) {
//     return (data & (1 << (len - 1))) ? (data | ((1 << to) - 1) << len) : data;
// }

DataType SEXT(ByteType data) {
    return (data & (1 << 7)) ? (data | 0xFFFFFF00) : data;
}
DataType SEXT(HalfType data) {
    return (data & (1 << 15)) ? (data | 0xFFFF0000) : data;
}

DataType ZEXT(ByteType data) {
    return data;
}
DataType ZEXT(HalfType data) {
    return data;
}

DataType SEXT(int data, int len) {
    int mask = (0xffffffffU) << len;
    return (data & (1 << (len - 1))) ? (data | mask) : (data & ~mask);
}
DataType ZEXT(int data, int len) {
    int mask = (0xffffffffU) << len;
    return data & ~mask;
}

HalfType Concat(ByteType low, ByteType high) {
    return low | (high << 8);
}

DataType Concat(HalfType low, HalfType high) {
    return low | (high << 16);
}

DataType Concat(int low, int len, int high) {
    return low | (high << len);
}

DataType Concat(ByteType l1, ByteType l2, ByteType h1, ByteType h2) {
    return l1 | (l2 << 8) | (h1 << 16) | (h2 << 24);
}

ByteType GetByte(DataType data, int pos) {
    return (data >> (pos * 8)) & 0xFF;
}

std::string OpcodeToStr(Optype opt) {
    switch (opt) {
    case LUI: return "LUI";
    case AUIPC: return "AUIPC";
    case JAL: return "JAL";
    case JALR: return "JALR";
    case BEQ: return "BEQ";
    case BNE: return "BNE";
    case BLT: return "BLT";
    case BGE: return "BGE";
    case BLTU: return "BLTU";
    case BGEU: return "BGEU";
    case LB: return "LB";
    case LH: return "LH";
    case LW: return "LW";
    case LBU: return "LBU";
    case LHU: return "LHU";
    case SB: return "SB";
    case SH: return "SH";
    case SW: return "SW";
    case ADDI: return "ADDI";
    case SLTI: return "SLTI";
    case SLTIU: return "SLTIU";
    case XORI: return "XORI";
    case ORI: return "ORI";
    case ANDI: return "ANDI";
    case SLLI: return "SLLI";
    case SRLI: return "SRLI";
    case SRAI: return "SRAI";
    case ADD: return "ADD";
    case SUB: return "SUB";
    case SLL: return "SLL";
    case SLT: return "SLT";
    case SLTU: return "SLTU";
    case XOR: return "XOR";
    case SRL: return "SRL";
    case SRA: return "SRA";
    case OR: return "OR";
    case AND: return "AND";
    default: return "NONE";
    }
}

} // namespace jasonfxz