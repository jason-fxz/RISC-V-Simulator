#include "units/arithmetic_logic_unit.h"
#include "config/types.h"
#include "simulator.h"
#include <stdexcept>


namespace jasonfxz {


void AddCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case ADD: case ADDI: case JALR:
            res = _.lhs + _.rhs;
            break;
        case SUB:
            res = _.lhs - _.rhs;
            break;
        default:
            throw std::runtime_error("Invalid optype in AddCalc");
        }
    } else {
        ++cur;
    }
}

void CampCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case BEQ:
            res = _.lhs == _.rhs;
            break;
        case BNE:
            res = _.lhs != _.rhs;
            break;
        case BGE:
            res = _.lhs >= _.rhs;
            break;
        case BGEU:
            res = (uint32_t)_.lhs >= (uint32_t)_.rhs;
            break;
        case SLT: case BLT: case SLTI:
            res = _.lhs < _.rhs;
            break;
        case SLTU: case BLTU: case SLTIU:
            res = (uint32_t)_.lhs < (uint32_t)_.rhs;
            break;
        default:
            throw std::runtime_error("Invalid optype in CampCalc");
        }
    } else {
        ++cur;
    }
}

void LogicCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case XOR: case XORI:
            res = _.lhs ^ _.rhs;
            break;
        case OR: case ORI:
            res = _.lhs | _.rhs;
            break;
        case AND: case ANDI:
            res = _.lhs & _.rhs;
            break;
        default:
            throw std::runtime_error("Invalid optype in LogicCalc");
        }
    } else {
        ++cur;
    }
}

void ShiftCalc::Calc() {
    if (cur == 0) return ;
    if (cur == latency) {
        switch (_.opt) {
        case SLL: case SLLI:
            res = _.lhs << _.rhs;
            break;
        case SRL: case SRLI:
            res = (uint32_t)_.lhs >> _.rhs;
            break;
        case SRA: case SRAI:
            res = _.lhs >> _.rhs;
            break;
        default:
            throw std::runtime_error("Invalid optype in ShiftCalc");
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
    if (cur_state->clear) {
        addCalc.clear();
        campCalc.clear();
        logicCalc.clear();
        shiftCalc.clear();
    }
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
        if (!cd_bus->e.insert({BusType::WriteBack, addCalc.res, addCalc._.rob_pos})) 
            throw std::runtime_error("cdBus full");
        addCalc.cur = 0;
    }
    if (campCalc.cur == campCalc.latency) {
        if (!cd_bus->e.insert({BusType::WriteBack, campCalc.res, campCalc._.rob_pos})) 
            throw std::runtime_error("cdBus full");
        campCalc.cur = 0;
    }
    if (logicCalc.cur == logicCalc.latency) {
        if (!cd_bus->e.insert({BusType::WriteBack, logicCalc.res, logicCalc._.rob_pos})) 
            throw std::runtime_error("cdBus full");
        logicCalc.cur = 0;
    }
    if (shiftCalc.cur == shiftCalc.latency) {
        if (!cd_bus->e.insert({BusType::WriteBack, shiftCalc.res, shiftCalc._.rob_pos})) 
            throw std::runtime_error("cdBus full");
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
