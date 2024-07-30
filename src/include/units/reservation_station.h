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

namespace jasonfxz {

class State;

struct RsInter {
    InsType ins;
    int rob_dst{0};
    int vj{0}, vk{0};
    int qj{-1}, qk{-1};
};

class ReservationStation : public BaseUnit {
  public:
    explicit ReservationStation(CdBus *cd_bus) : cd_bus(cd_bus) {}
    void Flush(State *cur_state) override;
    void Execute(State *cur_state, State *next_state) override;

  protected:
    void ExecuteALU(State *cur_state, State *next_state);
    void ExecuteLSB(State *cur_state, State *next_state);

  private:
    Carray<RsInter, MAX_RS_SIZE> alu_add_rs;
    Carray<RsInter, MAX_RS_SIZE> alu_camp_rs;
    Carray<RsInter, MAX_RS_SIZE> alu_logic_rs;
    Carray<RsInter, MAX_RS_SIZE> alu_shift_rs;
    Carray<RsInter, MAX_RS_SIZE> lsb_rs;
    CdBus *cd_bus;
};



}

#endif // RESERVATION_STATION_H