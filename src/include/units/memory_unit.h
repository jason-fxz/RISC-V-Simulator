/**
 * @file memory_unit.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief A unit that simulates the memory
 * @version 0.1
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MEMORY_UNIT_H
#define MEMORY_UNIT_H

#include "config/constant.h"
#include "config/types.h"
#include "base_unit.h"

namespace jasonfxz {

struct Memory {
  private:
    ByteType data[MAX_RAM_SIZE];  /// Memory
  public:
    Memory();
    void clear();
    ByteType &operator[](AddrType addr);
    ByteType ReadByte(AddrType addr);
    HalfType ReadHalf(AddrType addr);
    DataType ReadWord(AddrType addr);
    void WriteByte(AddrType addr, ByteType data);
    void WriteHalf(AddrType addr, HalfType data);
    void WriteWord(AddrType addr, DataType data);
};


class MemoryUnit : BaseUnit {

  private:
    Memory mem;  /// Memory
    

};

} // namespace jasonfxz


#endif