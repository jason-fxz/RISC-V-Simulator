#include "units/reservation_station.h"
#include "circuits/bus.h"
#include "config/constant.h"
#include "config/types.h"
#include "simulator.h"
#include "units/arithmetic_logic_unit.h"
#include "units/load_store_buffer.h"
#include "utils/utils.h"
#include <stdexcept>
#include <iostream>
#include <cstring>

namespace jasonfxz {

void ReservationStation::Print() {
    std::cerr << ">>> ALU_ADD_RS: " << alu_add_rs.count() << std::endl;
    for (int i = 0; i < alu_add_rs.size(); i++) {
        if (alu_add_rs.busy(i)) {
            std::cerr << "#" << i << ": " << OpcodeToStr(alu_add_rs[i].ins.opt) << " vj:" << alu_add_rs[i].vj << " vk:" <<
                      alu_add_rs[i].vk << " qj:" << alu_add_rs[i].qj << " qk:" << alu_add_rs[i].qk << std::endl;
        }
    }
    std::cerr << ">>> ALU_CAMP_RS: " << alu_camp_rs.count() << std::endl;
    for (int i = 0; i < alu_camp_rs.size(); i++) {
        if (alu_camp_rs.busy(i)) {
            std::cerr << "#" << i << ": " << OpcodeToStr(alu_camp_rs[i].ins.opt) << " vj:" << alu_camp_rs[i].vj << " vk:" <<
                      alu_camp_rs[i].vk << " qj:" << alu_camp_rs[i].qj << " qk:" << alu_camp_rs[i].qk << std::endl;
        }
    }
    std::cerr << ">>> ALU_LOGIC_RS: " << alu_logic_rs.count() << std::endl;
    for (int i = 0; i < alu_logic_rs.size(); i++) {
        if (alu_logic_rs.busy(i)) {
            std::cerr << "#" << i << ": " << OpcodeToStr(alu_logic_rs[i].ins.opt) << " vj:" << alu_logic_rs[i].vj << " vk:" <<
                      alu_logic_rs[i].vk << " qj:" << alu_logic_rs[i].qj << " qk:" << alu_logic_rs[i].qk << std::endl;
        }
    }
    std::cerr << ">>> ALU_SHIFT_RS: " << alu_shift_rs.count() << std::endl;
    for (int i = 0; i < alu_shift_rs.size(); i++) {
        if (alu_shift_rs.busy(i)) {
            std::cerr << "#" << i << ": " << OpcodeToStr(alu_shift_rs[i].ins.opt) << " vj:" << alu_shift_rs[i].vj << " vk:" <<
                      alu_shift_rs[i].vk << " qj:" << alu_shift_rs[i].qj << " qk:" << alu_shift_rs[i].qk << std::endl;
        }
    }
    std::cerr << ">>> LSB_RS: " << lsb_rs.count() << std::endl;
    for (int i = 0; i < lsb_rs.size(); i++) {
        if (lsb_rs.busy(i)) {
            std::cerr << "#" << i << ": " << OpcodeToStr(lsb_rs[i].ins.opt) << " vj:" << lsb_rs[i].vj << " vk:" << lsb_rs[i].vk <<
                      " qj:" << lsb_rs[i].qj << " qk:" << lsb_rs[i].qk << std::endl;
        }
    }
}


void ReservationStation::Flush(State *cur_state) {
    if (cur_state->clear) {
        for (int i = 0; i < 5; ++i) {
            rss[i].clear();
        }
    }
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
            case ADD: case SUB: case ADDI: case JALR:
                if (!alu_add_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_ADD_RS full");
                }
                break;
            case SLT: case SLTU: case SLTI: case SLTIU: case BEQ: case BNE: case BGE: case BGEU: case BLT: case BLTU:
                if (!alu_camp_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_CAMP_RS full");
                }
                break;
            case XOR: case OR: case AND:
            case XORI: case ORI: case ANDI:
                if (!alu_logic_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_LOGIC_RS full");
                }
                break;
            case SLL: case SRL: case SRA:
            case SLLI: case SRLI: case SRAI:
                if (!alu_shift_rs.insert(cur_state->rs_inter.second)) {
                    throw std::runtime_error("ALU_SHIFT_RS full");
                }
                break;
            default:
                // std::cerr << cur_state->rs_inter.second.ins.opt << std::endl;
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
    // Update Qj, Qk
    pair<bool, int> update[MAX_ROB_SIZE + 1];
    for (int i = 0; i < MAX_ROB_SIZE; ++i) {
        update[i] = {false, 0};
    }
    // Data From CdBUS
    for (const auto &it : cd_bus->e) if (it.first) {
            const auto &info = it.second;
            if (info.type == BusType::WriteBack || info.type == BusType::CommitReg) {
                update[info.pos] = {true, info.data};
            }
        }
    // Data From ROB Query
    if (cur_state->query_rob_id1_data.first != -1) {
        update[cur_state->query_rob_id1_data.first] = {true, cur_state->query_rob_id1_data.second};
    }
    if (cur_state->query_rob_id2_data.first != -1) {
        update[cur_state->query_rob_id2_data.first] = {true, cur_state->query_rob_id2_data.second};
    }
    // Check each Qj,Qk
    for (auto &rs : rss) {
        for (auto &jt : rs) {
            if (jt.first) {
                if (jt.second.qj != -1 && update[jt.second.qj].first) {
                    jt.second.vj = update[jt.second.qj].second;
                    jt.second.qj = -1;
                }
                if (jt.second.qk != -1 && update[jt.second.qk].first) {
                    // std::cerr << " fuck get qk " << std::endl;
                    jt.second.vk = update[jt.second.qk].second;
                    jt.second.qk = -1;
                }
            }
        }
    }
#ifdef DEBUG
    if (cur_state->enable_debug) {
        Print();
    }
#endif
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
                alu_add_rs.count();
                if (alu_add_rs[i].qj == -1 && alu_add_rs[i].qk == -1) {
                    next_state->alu_add_inter.first = true;
                    next_state->alu_add_inter.second = AluInter{
                        alu_add_rs[i].vj,
                        alu_add_rs[i].vk,
                        alu_add_rs[i].rob_pos,
                        alu_add_rs[i].ins.opt
                    };
                    alu_add_rs.remove(i);
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
                        alu_camp_rs[i].rob_pos,
                        alu_camp_rs[i].ins.opt
                    };
                    alu_camp_rs.remove(i);
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
                        alu_logic_rs[i].rob_pos,
                        alu_logic_rs[i].ins.opt
                    };
                    alu_logic_rs.remove(i);
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
                        alu_shift_rs[i].rob_pos,
                        alu_shift_rs[i].ins.opt
                    };
                    alu_shift_rs.remove(i);
                    break;
                }
            }
        }
}

void ReservationStation::ExecuteLSB(State *cur_state, State *next_state) {
    // Data already in RS  ====> address unit(摆烂了，这里直接算出来) =====> write back (ROB LSB)
    for (int i = 0; i < lsb_rs.size(); i++) {
        if (lsb_rs.busy(i)) {
            if (lsb_rs[i].ins.opc == OpClass::LOAD) {
                // rd <== mem[rs1(vj) + imm(vk)]
                if (lsb_rs[i].qj == -1 && lsb_rs[i].qk == -1) {
                    if (cd_bus->e.full()) throw std::runtime_error("CdBus Full");
                    cd_bus->e.insert(BusInter{
                        BusType::GetAddr,
                        lsb_rs[i].vj + lsb_rs[i].vk,
                        lsb_rs[i].rob_pos
                    });
                    lsb_rs.remove(i);
                    return ;
                }
            } else { // STORE
                // mem[rs1(vj) + imm(imm)] <== rs2(vk)
                if (lsb_rs[i].qj == -1 && lsb_rs[i].qk == -1) {
                    if (cd_bus->e.full()) throw std::runtime_error("CdBus Full");
                    cd_bus->e.insert(BusInter{
                        BusType::GetAddr,
                        lsb_rs[i].vj + lsb_rs[i].imm,
                        lsb_rs[i].rob_pos
                    });
                    if (cd_bus->e.full()) throw std::runtime_error("CdBus Full");
                    cd_bus->e.insert(BusInter{
                        BusType::WriteBack,
                        lsb_rs[i].vk,
                        lsb_rs[i].rob_pos
                    });
                    lsb_rs.remove(i);
                    return ;
                }
            }
        }
    }
}


} // namespace jasonfxz