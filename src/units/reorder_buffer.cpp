#include "circuits/bus.h"
#include "config/types.h"
#include "units/base_unit.h"
#include "utils/utils.h"
#include "units/reorder_buffer.h"
#include "simulator.h"
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iomanip>


namespace jasonfxz {
// class

void ReorderBuffer::Print() {
    std::cerr << ">>> ROB " << rob_queue.size()  << std::endl;
    for (const auto &it : rob_queue) {
        std::cerr << "> " << std::setw(4) << std::setfill('0') << std::hex << it.ins.ins_addr << " "
                  << std::dec << "#" << it.rob_pos << " Data:" << std::setw(10) << it.data << std::setfill(' ')
                  << " State:" << std::setw(7) << RobStateToStr(it.state)
                  << " Dest:" << std::setw(3) << it.dest
                  << " Ins: " << it.ins << std::endl;
    }
}

void ReorderBuffer::Flush(State *cur_state) {
    if (cur_state->clear) {
        rob_queue.clear();
        cur_state->rob_full = rob_queue.full();
        cur_state->rob_tail_pos = rob_queue.tail();
    }
    // handle issue
    if (cur_state->rob_inter.first) {
        if (rob_queue.full()) throw std::runtime_error("ROB is full");
        rob_queue.push(cur_state->rob_inter.second);
    }
    // lookup CdBus
    for (const auto &it : cd_bus->e) if (it.first) {
            const auto &info = it.second;
            switch (info.type) {
            case BusType::WriteBack:
                rob_queue[info.pos].state = RobState::Write;
                rob_queue[info.pos].data = info.data;
                break;
            // case BusType::Executing:
            //     rob_queue[info.pos].state = RobState::Exec;
            //     break;
            case BusType::StoreSuccess:
                assert(info.pos == rob_queue.front().rob_pos);
                rob_queue.pop();
                break;
            default: break;
            };
        }
    cur_state->rob_full = rob_queue.full();
    cur_state->rob_tail_pos = rob_queue.tail();
#ifdef DEBUG
    Print();
#endif
}

void ReorderBuffer::Commit(State *cur_state, State *next_state) {
    if (rob_queue.empty()) return ;
    auto &front = rob_queue.front();
    if (front.state != RobState::Write) return;
    // halt code
#ifdef DEBUG
    front.ins.Print(std::cout);
    next_state->have_commit = true;
#endif
    if (front.ins.opt == ADDI && front.ins.rd == reName::a0 && front.ins.rs1 == 0 && front.ins.imm == 255) {
        next_state->halt = true;
        return ;
    } else if (front.ins.opt == JALR) {
        // handle JALR
        next_state->regfile[front.ins.rd] = {front.ins.ins_addr + 4, -1};
        next_state->pc = front.data;
        next_state->wait = false;
        cd_bus->e.insert(BusInter{BusType::CommitReg,  front.data, front.rob_pos});
        rob_queue.pop();
    } else if (front.ins.opc == OpClass::BRANCH) {
        predictor->GetFeedBack(front.ins.ins_addr, front.data);
        if (front.ins.rd != front.data) {
            // Predicted Failed
#ifdef DEBUG
            std::cerr << "Predict Failed!!!!" << std::endl;
#endif
            next_state->clear = true;
            next_state->pc = front.data ? front.ins.ins_addr + front.ins.imm : front.ins.ins_addr + 4; // By Address Unit
        }
        rob_queue.pop();
    } else if (front.ins.opc == OpClass::ARITHI || front.ins.opc == OpClass::ARITHR
               || front.ins.opc == OpClass::LOAD) {
        if (cd_bus->e.full()) throw std::runtime_error("CdBus Full");
        // Modify NextCycle Reg
        // Why base on next_state?
        // Becasue the Issue will also modify the regfile !!!
        next_state->regfile[front.dest].data = front.data;
        if (next_state->regfile[front.dest].recorder == front.rob_pos) {
            next_state->regfile[front.dest].recorder = -1;
        }
        cd_bus->e.insert(BusInter{BusType::CommitReg,  front.data, front.rob_pos});
        rob_queue.pop();
    } else if (front.ins.opc == OpClass::STORE) {
        if (cd_bus->e.full()) throw std::runtime_error("CdBus Full");
        cd_bus->e.insert(BusInter{BusType::CommitMem, front.data, front.rob_pos});
        front.state = RobState::WaitSt;
        // Wait for Store Success in Flush
    } else {
        std::cerr << "FUCK commit" << front.ins.opt << std::endl;
        throw std::runtime_error("Unmatch Commit");
    }
}

void ReorderBuffer::Execute(State *cur_state, State *next_state) {
    // LookUp query_rob
    if (cur_state->query_rob_id1 != -1 && rob_queue.busy(cur_state->query_rob_id1)
        && rob_queue[cur_state->query_rob_id1].state == RobState::Write) {
        next_state->query_rob_id1_data = {cur_state->query_rob_id1, rob_queue[cur_state->query_rob_id1].data};
    } else {
        next_state->query_rob_id1_data.first = -1;
    }
    if (cur_state->query_rob_id2 != -1 && rob_queue.busy(cur_state->query_rob_id2)
        && rob_queue[cur_state->query_rob_id2].state == RobState::Write) {
        next_state->query_rob_id2_data = {cur_state->query_rob_id2, rob_queue[cur_state->query_rob_id2].data};
    } else {
        next_state->query_rob_id2_data.first = -1;
    }
    Commit(cur_state, next_state);
}

} // namespace jasonfxz