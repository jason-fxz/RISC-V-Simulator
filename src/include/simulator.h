/**
 * @file simulator.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-07-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <utility>
#include "config/types.h"
#include "units/arithmetic_logic_unit.h"
#include "units/register_file.h"
#include "units/reservation_station.h"
#include "units/load_store_buffer.h"

namespace jasonfxz {

using std::pair;


class State {

  public:
    int clock; // clock cycle

    // pc & ir
    int pc;
    // Instruction ins;


    // Register file
    RegisterFile reg;

    /// ALU
    pair<bool, AluInter> alu_add_inter;
    bool alu_add_busy{false};
    pair<bool, AluInter> alu_camp_inter;
    bool alu_camp_busy{false};
    pair<bool, AluInter> alu_logic_inter;
    bool alu_logic_busy{false};
    pair<bool, AluInter> alu_shift_inter;
    bool alu_shift_busy{false};
    
    // RS
    pair<bool, RsInter> rs_inter;
    bool rs_alu_add_full{false};
    bool rs_alu_camp_full{false};
    bool rs_alu_logic_full{false};
    bool rs_alu_shift_full{false};
    bool rs_lsb_full{false};

    // LSB
    pair<bool, LsbInter> lsb_load_inter;
    bool lsb_load_full{false};
    pair<bool, LsbInter> lsb_store_inter;
    bool lsb_store_full{false};

    



};


} // namespace jasonfxz


#endif