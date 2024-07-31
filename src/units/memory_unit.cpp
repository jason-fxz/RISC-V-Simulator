#include "units/memory_unit.h"
#include "config/constant.h"
#include "config/types.h"
#include "utils/utils.h"
#include <iostream>
#include <istream>

namespace jasonfxz {

Memory::Memory() {
    clear();
}

void Memory::clear() {
    for (int i = 0; i < MAX_RAM_SIZE; ++i) {
        data[i] = 0;
    }
}


ByteType &Memory::operator[](AddrType addr) {
    return data[addr];
}

ByteType Memory::ReadByte(AddrType addr) {
    return SEXT(data[addr]);
}
ByteType Memory::ReadByteU(AddrType addr) {
    return ZEXT(data[addr]);
}
HalfType Memory::ReadHalf(AddrType addr) {
    return SEXT(Concat(data[addr], data[addr + 1]));
}
HalfType Memory::ReadHalfU(AddrType addr) {
    return ZEXT(Concat(data[addr], data[addr + 1]));
}
DataType Memory::ReadWord(AddrType addr) {
    return Concat(data[addr], data[addr + 1], data[addr + 2], data[addr + 3]);
}

void Memory::WriteByte(AddrType addr, ByteType data) {
    this->data[addr] = data;
}
void Memory::WriteHalf(AddrType addr, HalfType data) {
    this->data[addr] = data & 0xff;
    this->data[addr + 1] = (data >> 8) & 0xff;
}
void Memory::WriteWord(AddrType addr, DataType data) {
    this->data[addr] = data & 0xff;
    this->data[addr + 1] = (data >> 8) & 0xff;
    this->data[addr + 2] = (data >> 16) & 0xff;
    this->data[addr + 3] = (data >> 24) & 0xff;
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

void Memory::Init(std::istream &is) {
    clear();
    // Read the memory from stdin
#ifdef DEBUG
    std::cerr << "Reading memory from istream" << std::endl;
#endif
    char input_str[10];
    AddrType cur_addr = 0x0;
    while (is >> input_str) {
#ifdef DEBUG
        std::cerr << input_str << " ";
#endif
        if (input_str[0] == '@') {
            cur_addr = 0x0;
            for (int i = 1; i <= 8; i++) {
                cur_addr = (cur_addr << 4) + HexToInt(input_str[i]);
            }
        } else {
            ByteType current_data = (HexToInt(input_str[0]) << 4) + HexToInt(input_str[1]);
            data[cur_addr++] = current_data;
        }
    }
#ifdef DEBUG
    std::cerr << "Reading memory DONE" << std::endl;
#endif
}



} // namespace jasonfxz