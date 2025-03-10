/**
 * @file instruction_unit.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-07-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef INSTRUCTION_UNIT_H
#define INSTRUCTION_UNIT_H

#include "base_unit.h"
#include "config/types.h"
#include "config/constant.h"
#include "circuits/cqueue.h"
#include "units/memory_unit.h"
#include <cstring>

namespace jasonfxz {

class Decoder {
  private:
    void DecodeR(InsType &ins);
    void DecodeI(InsType &ins);
    void DecodeS(InsType &ins);
    void DecodeB(InsType &ins);
    void DecodeU(InsType &ins);
    void DecodeJ(InsType &ins);

    void GetFunct3(InsType &ins);
    void GetFunct7(InsType &ins);
    void GetRd(InsType &ins);
    void GetRs1(InsType &ins);
    void GetRs2(InsType &ins);

  public:
    void Decode(InsType &ins);
};

class Predictor {
  private:
    char table[1 << 5];
    int count_tot{0}, count_suc{0};
  public:
    Predictor() {
        for (int i = 0; i < (1 << 5); ++i) {
            table[i] = 1;
        }
    }
    bool GetPrediction(AddrType pc);
    void GetFeedBack(AddrType pc, bool real, bool pred);
    ~Predictor() {
        auto rate = count_tot == 0 ? 100.0 : 100.0 * count_suc / count_tot;
        std::cerr << "Predictor " << count_suc << " / " << count_tot << " = " << rate << "%" << std::endl;
    }
};



class InstructionUnit : public BaseUnit {
  public:
    explicit InstructionUnit(Predictor *predictor, Memory *mem) : predictor(predictor), mem(mem) {}
    void Flush(State *cur_state) override;
    void Execute(State *cur_state, State *next_state) override;

  private:
    void Issue(State *cur_state, State *next_state);
    void FetchDecode(State *cur_state, State *next_state);

    Decoder decoder;
    Predictor *predictor;
    Memory *mem;
    Cqueue<InsType, MAX_INS_SIZE> ins_queue;
};


} // namespace jasonfxz




#endif // INSTRUCTION_UNIT_H

