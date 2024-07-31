/**
 * @file reservation_station.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef RESERVATION_STATION_H
#define RESERVATION_STATION_H

#include "config/types.h"
#include "config/constant.h"
#include "base_unit.h"
#include "circuits/bus.h"
#include "circuits/carray.h"
#include <ostream>

namespace jasonfxz {

class State;

struct RsInter {
    InsType ins;
    int rob_pos{0};
    // 两个值时（ARITHR / ARITHI / LOAD）， vj, vk 为操作数（rs1 + rs2 / rs1 + imm）。
    // 三个值时（STORE / BRANCH）， vj, vk 为 rs1, rs2； imm 为 offset
    int vj{0}, vk{0};
    int qj{-1}, qk{-1};
    int imm{0};

    friend std::ostream &operator<<(std::ostream &os, RsInter &a) {
        os << "ins: " << a.ins << ", rob_pos: " << a.rob_pos << ", vj: " << a.vj << ", vk: " << a.vk << ", qj: " << a.qj << ", qk: " << a.qk << ", imm: " << a.imm;
        return os;
    }
};

class ReservationStation : public BaseUnit {
  public:
    explicit ReservationStation(CdBus *cd_bus) : cd_bus(cd_bus) {}
    void Flush(State *cur_state) override;
    void Execute(State *cur_state, State *next_state) override;
    
  protected:
    void ExecuteALU(State *cur_state, State *next_state);
    void ExecuteLSB(State *cur_state, State *next_state);
    void Print();
  private:
    Carray<RsInter, MAX_RS_SIZE> rss[5];
    Carray<RsInter, MAX_RS_SIZE> &alu_add_rs = rss[0];
    Carray<RsInter, MAX_RS_SIZE> &alu_camp_rs = rss[1];
    Carray<RsInter, MAX_RS_SIZE> &alu_logic_rs = rss[2];
    Carray<RsInter, MAX_RS_SIZE> &alu_shift_rs = rss[3];
    Carray<RsInter, MAX_RS_SIZE> &lsb_rs = rss[4];
    CdBus *cd_bus;
};



}

#endif // RESERVATION_STATION_H