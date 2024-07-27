/**
 * @file utils.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief Some utility functions
 * @version 0.1
 * @date 2024-07-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <string>
#include "../config/types.h"

namespace jasonfxz {
    
DataType ZEXT(ByteType data); // Zero-extend
DataType SEXT(ByteType data); // Sign-extend
DataType ZEXT(HalfType data); // Zero-extend
DataType SEXT(HalfType data); // Sign-extend

DataType SEXT(int data, int len);
DataType ZEXT(int data, int len);


HalfType Concat(ByteType low, ByteType high); // Concatenate two 8-bit data to a 16-bit data 
DataType Concat(HalfType low, HalfType high); // Concatenate two 16-bit data to a 32-bit data
DataType Concat(ByteType l1, ByteType l2, ByteType h1, ByteType h2); // Concatenate four 8-bit data to a 32-bit data
DataType Concat(int low, int len, int high); // Concatenate two data with different length


ByteType GetByte(DataType data, int pos); // Get the byte at the position pos

std::string OpcodeToStr(Optype opt); // Convert the opcode to string



} // namespace jasonfxz


#endif // UTILS_H