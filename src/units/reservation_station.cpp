#include "units/reservation_station.h"
#include "config/types.h"
#include "simulator.h"
#include "units/arithmetic_logic_unit.h"
#include "units/load_store_buffer.h"
#include <stdexcept>

namespace jasonfxz {

void ReservationStation::Flush(State *cur_state) {
    if (cur_state->rs_inter.first) {
        if (cur_state->rs_inter.second.ins.opc == OpClass::LOAD
            || cur_state->rs_inter.second.ins.opc == OpClass::STORE) {
            // Load or Store
            if (!lsb_rs.insert(cur_state->rs_inter.second)) {
                throw std::runtime_error("LS_RS full");
            }
        } else {
            // ALU
            switch (cur_state->rs_inter.second.ins.opt) {
            case ADD: case SUB:
                if (!alu_add_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_ADD_RS full");
                }
                break;
            case SLT: case SLTU:
                if (!alu_camp_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_CAMP_RS full");
                }
                break;
            case XOR: case OR: case AND:
                if (!alu_logic_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_LOGIC_RS full");
                }
                break;
            case SLL: case SRL: case SRA:
                if (!alu_shift_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_SHIFT_RS full");
                }
                break;
            default:
                throw std::runtime_error("Unknown op class");
            }
        }
    }
    // Set the full flag
    cur_state->rs_alu_add_full = alu_add_rs.full();
    cur_state->rs_alu_camp_full = alu_camp_rs.full();
    cur_state->rs_alu_logic_full = alu_logic_rs.full();
    cur_state->rs_alu_shift_full = alu_shift_rs.full();
    cur_state->rs_lsb_full = lsb_rs.full();
}

void ReservationStation::Execute(State *cur_state, State *next_state) {
    ExecuteALU(cur_state, next_state);
    ExecuteLSB(cur_state, next_state);
}

void ReservationStation::ExecuteALU(State *cur_state, State *next_state) {
    // ALU_ADD
    if (!cur_state->alu_add_busy)
        for (int i = 0; i < alu_add_rs.size(); i++) {
            if (alu_add_rs.busy(i)) {
                if (alu_add_rs[i].qj == -1 && alu_add_rs[i].qk == -1) {
                    next_state->alu_add_inter.first = true;
                    next_state->alu_add_inter.second = AluInter{
                        alu_add_rs[i].vj,
                        alu_add_rs[i].vk,
                        alu_add_rs[i].rob_dst,
                        alu_add_rs[i].ins.opt
                    };
                    alu_add_rs.clean(i);
                    break;
                }
            }
        }
    // ALU_CAMP
    if (!cur_state->alu_camp_busy)
        for (int i = 0; i < alu_camp_rs.size(); i++) {
            if (alu_camp_rs.busy(i)) {
                if (alu_camp_rs[i].qj == -1 && alu_camp_rs[i].qk == -1) {
                    next_state->alu_camp_inter.first = true;
                    next_state->alu_camp_inter.second = AluInter{
                        alu_camp_rs[i].vj,
                        alu_camp_rs[i].vk,
                        alu_camp_rs[i].rob_dst,
                        alu_camp_rs[i].ins.opt
                    };
                    alu_camp_rs.clean(i);
                    break;
                }
            }
        }
    // ALU_LOGIC
    if (!cur_state->alu_logic_busy)
        for (int i = 0; i < alu_logic_rs.size(); i++) {
            if (alu_logic_rs.busy(i)) {
                if (alu_logic_rs[i].qj == -1 && alu_logic_rs[i].qk == -1) {
                    next_state->alu_logic_inter.first = true;
                    next_state->alu_logic_inter.second = AluInter{
                        alu_logic_rs[i].vj,
                        alu_logic_rs[i].vk,
                        alu_logic_rs[i].rob_dst,
                        alu_logic_rs[i].ins.opt
                    };
                    alu_logic_rs.clean(i);
                    break;
                }
            }
        }
    // ALU_SHIFT
    if (!cur_state->alu_shift_busy)
        for (int i = 0; i < alu_shift_rs.size(); i++) {
            if (alu_shift_rs.busy(i)) {
                if (alu_shift_rs[i].qj == -1 && alu_shift_rs[i].qk == -1) {
                    next_state->alu_shift_inter.first = true;
                    next_state->alu_shift_inter.second = AluInter{
                        alu_shift_rs[i].vj,
                        alu_shift_rs[i].vk,
                        alu_shift_rs[i].rob_dst,
                        alu_shift_rs[i].ins.opt
                    };
                    alu_shift_rs.clean(i);
                    break;
                }
            }
        }
}

void ReservationStation::ExecuteLSB(State *cur_state, State *next_state) {
    // Data already in RS  ====> address unit(摆烂了，这里直接算出来) =====> write back (ROB LSB)
    for (int i = 0; i < lsb_rs.size(); i++) {
        if (lsb_rs.busy(i)) {
            if (lsb_rs[i].qj == -1 && lsb_rs[i].qk == -1) {
                int addr = lsb_rs[i].vj + lsb_rs[i].vk; // Address Unit
                if (!cd_bus->e.insert({BusType::WriteBack, addr, lsb_rs[i].rob_dst})) {
                    throw std::runtime_error("CD BUS full");
                }
            }
        }
    }
}


} // namespace jasonfxz