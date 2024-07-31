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

#include <istream>
#include <ostream>
#include <utility>
#include "config/types.h"
#include "units/arithmetic_logic_unit.h"
#include "units/base_unit.h"
#include "units/instruction_unit.h"
#include "units/memory_unit.h"
#include "units/register_file.h"
#include "units/reservation_station.h"
#include "units/load_store_buffer.h"
#include "units/reorder_buffer.h"

namespace jasonfxz {

using std::pair;


class State {
  public:
    // For Debug
    bool have_commit{false};


    int clock; // clock cycle

    bool halt{false}; // halt flag (terminate the simulation)
    bool wait{false}; // wait flag (Instruction fetch STOP)
    bool clear{false}; // clear flag (Clear All because of prediction failed)

    // pc & ir
    AddrType pc;
    WordType ir;

    // Ins
    bool ins_queue_full{false};
    pair<bool, InsType> ins{false, InsType()}; // Decoded by decoder in last cycle

    // Register file
    RegisterFile regfile;

    /// ALU
    pair<bool, AluInter> alu_add_inter{false, AluInter()};
    bool alu_add_busy{false};
    pair<bool, AluInter> alu_camp_inter{false, AluInter()};
    bool alu_camp_busy{false};
    pair<bool, AluInter> alu_logic_inter{false, AluInter()};
    bool alu_logic_busy{false};
    pair<bool, AluInter> alu_shift_inter{false, AluInter()};
    bool alu_shift_busy{false};

    // RS
    pair<bool, RsInter> rs_inter{false, RsInter()};
    bool rs_alu_add_full{false};
    bool rs_alu_camp_full{false};
    bool rs_alu_logic_full{false};
    bool rs_alu_shift_full{false};
    bool rs_lsb_full{false};

    // LSB
    pair<bool, LsbInter> lsb_load_inter{false, LsbInter()};
    bool lsb_load_full{false};
    pair<bool, LsbInter> lsb_store_inter{false, LsbInter()};
    bool lsb_store_full{false};

    // ROB
    pair<bool, RobInter> rob_inter{false, RobInter()};
    bool rob_full{false};
    int rob_tail_pos{0};

    int query_rob_id1{-1}, query_rob_id2{-1};
    // pos, data
    pair<int, int> query_rob_id1_data{-1, 0}, query_rob_id2_data{-1, 0};

  public:
    void clear_state();
};



class Simulator {

  public:
    Simulator();
    ~Simulator();
    void Init(std::istream &is);
    ReturnType Run();
  private:
    void Flush();
    void Execute();
  private:
    State *cur_state, *next_state;
    CdBus *cd_bus;
    Predictor *predictor;
    BaseUnit *units[5];
    Memory *mem;
  public:
    void PrintReg(std::ostream &os, RegisterFile *regfile);
    void PrintRegFile(std::ostream &os, RegisterFile *regfile);
    void PrintRegHelp(std::ostream &os);
    void PrintMem(std::ostream &os, AddrType addr, int len);
    void PrintCdBus(std::ostream &os);
};


} // namespace jasonfxz


#endif