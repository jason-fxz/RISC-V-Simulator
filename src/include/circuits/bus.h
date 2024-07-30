/**
 * @file bus.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef BUS_H
#define BUS_H

#include "config/types.h"
#include "carray.h"

namespace jasonfxz {

enum BusType {
    WriteBack,  // Write back to ROB (Load / ALU)
    Commit,     // ROB Commit to register file / memory
    StoreSuccess,  // Store success
};

struct BusInter {
    BusType type;
    int data;
    int pos;
};


template <size_t width>
struct Bus {
    Carray<BusInter, width> e;
};

using CdBus = Bus<8>;

}



#endif // BUS_H