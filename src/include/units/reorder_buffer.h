/**
 * @file reorder_buffer.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief ROB
 * @version 0.1
 * @date 2024-07-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include "base_unit.h"
#include "config/types.h"
#include "config/constant.h"
#include "circuits/bus.h"
#include "circuits/cqueue.h"

namespace jasonfxz {

class Predictor;

enum class RobState {
    Issue, // Issue
    Exec,  // Execute
    Write, // Write back
    WaitSt,// Wait for STORE
};

struct RobInter {
    InsType ins;
    RobState state;
    int rob_pos;
    int dest;
    int data;
};

class State;

class ReorderBuffer : public BaseUnit {
  public:
    explicit ReorderBuffer(CdBus *cd_bus, Predictor *predictor) : cd_bus(cd_bus), predictor(predictor) {}
    void Flush(State *cur_state) override;
    void Execute(State *cur_state, State *next_state) override;
    void Print();

  private:
    void Commit(State *cur_state, State *next_state);
  private:
    Cqueue<RobInter, MAX_ROB_SIZE> rob_queue;
    CdBus *cd_bus;
    Predictor *predictor;
};

} // namespace jasonfxz


#endif