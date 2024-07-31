#include "simulator.h"
#include "circuits/bus.h"
#include "config/types.h"
#include "units/instruction_unit.h"
#include "units/register_file.h"
#include "units/memory_unit.h"
#include "units/reservation_station.h"
#include "units/reorder_buffer.h"
#include "units/load_store_buffer.h"
#include "units/arithmetic_logic_unit.h"
#include "utils/utils.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <ratio>


namespace jasonfxz {


Simulator::Simulator() {
    cd_bus = new CdBus();
    predictor = new Predictor();
    mem = new Memory();
    units[0] = new InstructionUnit(predictor, mem);
    units[1] = new ReorderBuffer(cd_bus, predictor);
    units[2] = new ArithmeticLogicUnit(cd_bus);
    units[3] = new ReservationStation(cd_bus);
    units[4] = new LoadStoreBuffer(cd_bus, mem);
    cur_state = nullptr;
    next_state = nullptr;
}

Simulator::~Simulator() {
    delete cd_bus;
    delete predictor;
    delete mem;
    for (int i = 0; i < 5; i++) {
        delete units[i];
    }
    delete cur_state;
}

void Simulator::Init(std::istream &is) {
    mem->Init(is);
    cur_state = nullptr;
    next_state = new State;
    next_state->pc = 0;
    next_state->clock = 0;
}
void Simulator::Flush() {
    delete cur_state;
    cur_state = next_state;
    cur_state->regfile[reName::zero] = {0, -1};
    if (cur_state->clear) {
        cd_bus->e.clear();
        cur_state->wait = false;
        cur_state->clear_state();
        for (auto &rg : cur_state->regfile.reg) {
            rg.recorder = -1;
        }
    }
    for (auto &unit : units) {
        unit->Flush(cur_state);
    }
    cd_bus->e.clear();
}

void Simulator::Execute() {
    next_state = new State;
    next_state->pc = cur_state->pc;
    next_state->clock = cur_state->clock + 1;
    next_state->wait = cur_state->wait;
    next_state->regfile = cur_state->regfile;
    for (auto &unit : units) {
        unit->Execute(cur_state, next_state);
    }
}


void State::clear_state() {
    ins.first = alu_add_inter.first = alu_camp_inter.first = alu_logic_inter.first = alu_shift_inter.first = false;
    rs_inter.first = lsb_load_inter.first = lsb_store_inter.first = false;
    rob_inter.first = false;
    query_rob_id1 = query_rob_id2 = -1;
}

ReturnType Simulator::Run() {
#ifdef DEBUG
    PrintRegHelp(std::cout);
#endif 
    while (true) {
#ifdef DEBUG
        std::cerr << std::dec <<  "******************* clock " << next_state->clock << " wait: "  << next_state->wait << " halt: " << next_state->halt << " cur_pc:" << std::hex <<
                  next_state->pc << std::dec << std::endl;
        PrintRegFile(std::cerr, &next_state->regfile);
#endif 
        Flush();
        
        if (cur_state->halt) {
            return cur_state->regfile[reName::a0].data & 255U;
        }
        Execute();
#ifdef DEBUG
        
        if (next_state->have_commit) {
            PrintReg(std::cout, &next_state->regfile);
        }
        PrintCdBus(std::cerr);
#endif
    }
}



void Simulator::PrintRegHelp(std::ostream &os) {
    os << "+**************************************************************************+" << std::endl;
    os << "+  #   + Name + Description           +  #   + Name + Description          +" << std::endl;
    os << "+******+******+***********************+******+******+**********************+" << std::endl;
    os << "+ x0   + zero + Constant 0            + x16  + a6   + Args                 +" << std::endl;
    os << "+ x1   + ra   + Return Address        + x17  + a7   +                      +" << std::endl;
    os << "+ x2   + sp   + Stack Pointer         + x18  + s2   + Saved Registers      +" << std::endl;
    os << "+ x3   + gp   + Global Pointer        + x19  + s3   +                      +" << std::endl;
    os << "+ x4   + tp   + Thread Pointer        + x20  + s4   +                      +" << std::endl;
    os << "+ x5   + t0   + Temporary Registers   + x21  + s5   +                      +" << std::endl;
    os << "+ x6   + t1   +                       + x22  + s6   +                      +" << std::endl;
    os << "+ x7   + t2   +                       + x23  + s7   +                      +" << std::endl;
    os << "+ x8   + s0   + Saved Registers       + x24  + s8   +                      +" << std::endl;
    os << "+ x9   + s1   +                       + x25  + s9   +                      +" << std::endl;
    os << "+ x10  + a0   + Function Arguments or + x26  + s10  +                      +" << std::endl;
    os << "+ x11  + a1   +  Return Values        + x27  + s11  +                      +" << std::endl;
    os << "+ x12  + a2   + Function Arguments    + x28  + t3   + Temporaries          +" << std::endl;
    os << "+ x13  + a3   +                       + x29  + t4   +                      +" << std::endl;
    os << "+ x14  + a4   +                       + x30  + t5   +                      +" << std::endl;
    os << "+ x15  + a5   +                       + x31  + t6   +                      +" << std::endl;
    os << "+**************************************************************************+" << std::endl;
}

void Simulator::PrintCdBus(std::ostream &os) {
    for (auto &it : cd_bus->e) {
        if (it.first) {
            os << std::dec << "BUS >>> Type: " << BusTypeToStr(it.second.type) << " Data: " << it.second.data << " RobPos: "
                      << it.second.pos <<
                      std::endl;
        }
    }
}

void Simulator::PrintRegFile(std::ostream &os, RegisterFile *regfile) {
    os << "Register File:" << std::endl;
    os << "+-----+----------+-------------+-----+-----+----------+-------------+-----+" << std::endl;
    os << "| Reg |      Hex |         Dec |  #  | Reg |      Hex |         Dec |  #  |" << std::endl;
    os << "+-----+----------+-------------+-----+-----+----------+-------------+-----+" << std::endl;
    for (int i = 0; i < REG_FILE_SIZE / 2; ++i) {
        os << "| x" << std::setfill(' ') << std::left << std::setw(2) << i << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << regfile->reg[i].data
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << regfile->reg[i].data << " | "
                  << std::setw(3) << regfile->reg[i].recorder << " | ";
        int j = i | REG_FILE_SIZE / 2;
        os << "x" <<  std::left << std::setw(2) << j << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << regfile->reg[j].data
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << regfile->reg[j].data << " | "
                  << std::setw(3) << regfile->reg[j].recorder << " | " << std::endl;
    }
    os << "+-----+----------+-------------+-----+-----+----------+-------------+-----+" << std::endl;
}

void Simulator::PrintReg(std::ostream &os, RegisterFile *regfile) {
    os << "Register File:" << std::endl;
    os << "+-----+----------+-------------+-----+----------+-------------+" << std::endl;
    os << "| Reg |      Hex |         Dec | Reg |      Hex |         Dec |" << std::endl;
    os << "+-----+----------+-------------+-----+----------+-------------+" << std::endl;
    for (int i = 0; i < REG_FILE_SIZE / 2; ++i) {
        os << "| x" << std::setfill(' ') << std::left << std::setw(2) << i << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << regfile->reg[i].data
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << regfile->reg[i].data << " | ";
        int j = i | REG_FILE_SIZE / 2;
        os << "x" <<  std::left << std::setw(2) << j << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << regfile->reg[j].data
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << regfile->reg[j].data << " |" << std::endl;
    }
    os << "+-----+----------+-------------+-----+----------+-------------+" << std::endl;
}

void Simulator::PrintMem(std::ostream &os, AddrType addr, int len) {
    os << "Memory:" << std::endl;
    os << "+-----+----------+-------------+" << std::endl;
    os << "| Addr|      Hex |         Dec |" << std::endl;
    os << "+-----+----------+-------------+" << std::endl;
    for (int i = 0; i < len; ++i) {
        os << "| " << std::setfill(' ') << std::left << std::setw(4) << std::hex << addr + i << " | " << std::right
                  << std::setw(8) << std::hex << std::setfill('0') << (int)mem->data[addr + i]
                  << " | " << std::setw(11) << std::dec << std::setfill(' ') << (int)mem->data[addr + i] << " |" << std::endl;
    }
    os << "+-----+----------+-------------+" << std::endl;
}


}


/*
PC: 1008 IR: 00f54533

*/