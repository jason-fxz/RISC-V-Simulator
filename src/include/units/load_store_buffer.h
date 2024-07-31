/**
 * @file load_store_buffer.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-07-29
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef LOAD_STORE_BUFFER_H
#define LOAD_STORE_BUFFER_H


#include "base_unit.h"
#include "circuits/bus.h"
#include "circuits/cqueue.h"
#include "config/types.h"
#include "config/constant.h"
#include "units/memory_unit.h"

namespace jasonfxz {

using std::pair;

class State;

struct LsbInter {
    OpClass opc;
    OpType opt; // load or store
    int rob_pos;
    AddrType addr{0};
    bool addr_ready{0}; // 0: not ready, 1: ready; 
};


class LoadStoreBuffer : public BaseUnit {
  public:
    explicit LoadStoreBuffer(CdBus *cd_bus, Memory *mem) : cd_bus(cd_bus), mem(mem) {}
    void Flush(State *cur_state) override;
    void Execute(State *cur_state, State *next_state) override;
    

  private:
    int load_latency = 3;
    int store_latency = 3;

    int load_counter = 0;
    int store_counter = 0;

    bool store_enable = false; // Wait for ROB commit
    int store_data;
    
    int load_enable_level = 0; // Wait for store
  
    Cqueue<pair<int, LsbInter>, MAX_LSB_SIZE> load_queue, store_queue;
    CdBus *cd_bus;
    Memory *mem;
};



} // namespace jasonfxz



#endif // LOAD_STORE_BUFFER_H