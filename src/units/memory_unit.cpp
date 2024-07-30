#include "units/memory_unit.h"
#include "config/constant.h"
#include "config/types.h"
#include "utils/utils.h"

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





} // namespace jasonfxz