#include "units/arithmetic_logic_unit.h"
#include "config/types.h"
#include "simulator.h"
#include <stdexcept>


namespace jasonfxz {


void AddCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case OpType::ADD:
            res = _.lhs + _.rhs;
            break;
        case OpType::SUB:
            res = _.lhs - _.rhs;
            break;
        default:
            throw "Invalid optype in AddCalc";
        }
    } else {
        ++cur;
    }
}

void CampCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case OpType::SLT:
            res = _.lhs < _.rhs;
            break;
        case OpType::SLTU:
            res = (uint32_t)_.lhs < (uint32_t)_.rhs;
            break;
        default:
            throw "Invalid optype in CampCalc";
        }
    } else {
        ++cur;
    }
}

void LogicCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case OpType::XOR:
            res = _.lhs ^ _.rhs;
            break;
        case OpType::OR:
            res = _.lhs | _.rhs;
            break;
        case OpType::AND:
            res = _.lhs & _.rhs;
            break;
        default:
            throw "Invalid optype in LogicCalc";
        }
    } else {
        ++cur;
    }
}

void ShiftCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case OpType::SLL:
            res = _.lhs << _.rhs;
            break;
        case OpType::SRL:
            res = (uint32_t)_.lhs >> _.rhs;
            break;
        case OpType::SRA:
            res = _.lhs >> _.rhs;
            break;
        default:
            throw "Invalid optype in ShiftCalc";
        }
    } else {
        ++cur;
    }
}

void AddCalc::Flush(State *cur_state) {
    if (cur_state->alu_add_inter.first) {
        cur = 1;
        _ = cur_state->alu_add_inter.second;
    }
    cur_state->alu_add_busy = (0 < cur && cur < latency); 
}

void CampCalc::Flush(State *cur_state) {
    if (cur_state->alu_camp_inter.first) {
        cur = 1;
        _ = cur_state->alu_camp_inter.second;
    }
    cur_state->alu_camp_busy = (0 < cur && cur < latency);
}

void LogicCalc::Flush(State *cur_state) {
    if (cur_state->alu_logic_inter.first) {
        cur = 1;
        _ = cur_state->alu_logic_inter.second;
    }
    cur_state->alu_logic_busy = (0 < cur && cur < latency);
}

void ShiftCalc::Flush(State *cur_state) {
    if (cur_state->alu_shift_inter.first) {
        cur = 1;
        _ = cur_state->alu_shift_inter.second;
    }
    cur_state->alu_shift_busy = (0 < cur && cur < latency);
}


void ArithmeticLogicUnit::Flush(State *cur_state) {
    addCalc.Flush(cur_state);
    campCalc.Flush(cur_state);
    logicCalc.Flush(cur_state);
    shiftCalc.Flush(cur_state);
}

void ArithmeticLogicUnit::Execute(State *cur_state, State *next_state) {
    addCalc.Calc();
    campCalc.Calc();
    logicCalc.Calc();
    shiftCalc.Calc();
    if (addCalc.cur == addCalc.latency) {
        int index = cd_bus->e.getpos();
        if (index == -1) throw std::runtime_error("Bus full");
        cd_bus->e.set(index, {BusType::WriteBack, addCalc.res, addCalc._.rob_dst});
        addCalc.cur = 0;
    }
    if (campCalc.cur == campCalc.latency) {
        int index = cd_bus->e.getpos();
        if (index == -1) throw std::runtime_error("Bus full");
        cd_bus->e.set(index, {BusType::WriteBack, campCalc.res, campCalc._.rob_dst});
        campCalc.cur = 0;
    }
    if (logicCalc.cur == logicCalc.latency) {
        int index = cd_bus->e.getpos();
        if (index == -1) throw std::runtime_error("Bus full");
        cd_bus->e.set(index, {BusType::WriteBack, logicCalc.res, logicCalc._.rob_dst});
        logicCalc.cur = 0;
    }
    if (shiftCalc.cur == shiftCalc.latency) {
        int index = cd_bus->e.getpos();
        if (index == -1) throw std::runtime_error("Bus full");
        cd_bus->e.set(index, {BusType::WriteBack, shiftCalc.res, shiftCalc._.rob_dst});
        shiftCalc.cur = 0;
    }
}


ArithmeticLogicUnit::ArithmeticLogicUnit(CdBus *cd_bus) {
    this->cd_bus = cd_bus;
    addCalc.latency = 1;
    campCalc.latency = 1;
    logicCalc.latency = 1;
    shiftCalc.latency = 1;
}





} // namespace jasonfxz
