/**
 * @file register_file.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief register file
 * @version 0.1
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include "config/constant.h"
#include "config/types.h"
namespace jasonfxz {

struct Register {
    DataType data     =  0;  /// Register data
    int      recorder = -1;  /// Record the instruction (ROB id) that writes to this register
};

struct RegisterFile {
    Register regs[REG_FILE_SIZE];  /// Register file
};

}


#endif // REGISTER_FILE_H