/**
 * @file arithmetic_logic_unit.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-07-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef ARITHMETIC_LOGIC_UNIT_H
#define ARITHMETIC_LOGIC_UNIT_H


#include <cstdint>
#include "base_unit.h"
#include "config/types.h"
#include "circuits/bus.h"

namespace jasonfxz {


struct AluInter {
    int lhs, rhs;
    int rob_dst;
    OpType opt;
};


class BaseCalc {
  public:
    AluInter _;
    int latency{1};
    int cur{0};
    int res{0};
  public:
    virtual void Calc() = 0;
    virtual void Flush(State *cur_state) = 0;
};

// ADD / SUB
class AddCalc : public BaseCalc {
  public:
    void Calc() override;
    void Flush(State *cur_state) override;
};

// SLT / SLTU
class CampCalc : public BaseCalc {
  public:
    void Calc() override;
    void Flush(State *cur_state) override;

};

// XOR / OR / AND
class LogicCalc : public BaseCalc {
  public:
    void Calc() override;
    void Flush(State *cur_state) override;

};

// SLL / SRL / SRA
class ShiftCalc : public BaseCalc {
  public:
    void Calc() override;
    void Flush(State *cur_state) override;
    
};



class ArithmeticLogicUnit : public BaseUnit {
  private:
    AddCalc addCalc;
    CampCalc campCalc;
    LogicCalc logicCalc;
    ShiftCalc shiftCalc;

    CdBus *cd_bus;
  public:
    explicit ArithmeticLogicUnit(CdBus *cd_bus);
    void Flush(State *cur_state) override;
    void Execute(State *cur_state, State *next_state) override;
};

} // namespace jasonfxz



#endif // ARITHMETIC_LOGIC_UNIT_H