#include "units/instruction_unit.h"
#include "config/types.h"
#include "simulator.h"
#include "units/load_store_buffer.h"
#include "units/reorder_buffer.h"
#include "units/reservation_station.h"
#include "utils/utils.h"
#include <stdexcept>

namespace jasonfxz {

void Decoder::Decode(InsType &ins) {
    ins.opcode = GetByte(ins.ir, 0) & 0x7F;  /// [6:0]
    switch (ins.opcode) {
    case 0b011'0111:
    case 0b001'0111:
        DecodeU(ins); break;
    case 0b110'1111:
        DecodeJ(ins); break;
    case 0b110'0111:
    case 0b000'0011:
    case 0b001'0011:
        DecodeI(ins); break;
    case 0b110'0011:
        DecodeB(ins); break;
    case 0b010'0011:
        DecodeS(ins); break;
    case 0b011'0011:
        DecodeR(ins); break;
    };
}

void Decoder::GetFunct3(InsType &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.funct3 = (GetByte(ins.ir, 1) & 0b0111'0000U) >> 4; /// [14:12]
}
void Decoder::GetFunct7(InsType &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.funct7 = (GetByte(ins.ir, 3) & 0b1111'1110U) >> 1; /// [31:25]
}
void Decoder::GetRd(InsType &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.rd = (ins.ir & 0b1111'1000'0000U) >> 7; /// [11:7]
}
void Decoder::GetRs1(InsType &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.rs1 = (ins.ir & 0b1111'1000'0000'0000'0000U) >> 15; /// [19:15]
}

void Decoder::GetRs2(InsType &ins) {
    // [31:24] [23:16] [15:8] [7:0]
    ins.rs2 = (ins.ir & 0b0001'1111'0000'0000'0000'0000'0000U) >> 20; /// [24:20]
}


void Decoder::DecodeR(InsType &ins) {
    GetFunct7(ins);
    GetRs2(ins);
    GetRs1(ins);
    GetFunct3(ins);
    GetRd(ins);
    switch (ins.funct3) {
    case 0b000:
        if (ins.funct7 == 0b000'0000) ins.opt = ADD;
        if (ins.funct7 == 0b010'0000) ins.opt = SUB;
        break;
    case 0b111:
        if (ins.funct7 == 0b000'0000) ins.opt = AND;
        break;
    case 0b110:
        if (ins.funct7 == 0b000'0000) ins.opt = OR;
        break;
    case 0b100:
        if (ins.funct7 == 0b000'0000) ins.opt = XOR;
        break;
    case 0b001:
        if (ins.funct7 == 0b000'0000) ins.opt = SLL;
        break;
    case 0b101:
        if (ins.funct7 == 0b000'0000) ins.opt = SRL;
        if (ins.funct7 == 0b010'0000) ins.opt = SRA;
        break;
    case 0b010:
        if (ins.funct7 == 0b000'0000) ins.opt = SLT;
        break;
    case 0b011:
        if (ins.funct7 == 0b000'0000) ins.opt = SLTU;
        break;
    }
    ins.opc = OpClass::ARITHR;
}

void Decoder::DecodeI(InsType &ins) {
    GetRs1(ins);
    GetFunct3(ins);
    GetRd(ins);
    GetFunct7(ins);
    // 31 - 20 Sign-Extend
    ins.imm = SEXT(ins.ir >> 20, 12);
    switch (ins.opcode) {
    case 0b001'0011:
        switch (ins.funct3) {
        case 0b000:
            ins.opt = ADDI, ins.opc = OpClass::ARITHI; break;
        case 0b111:
            ins.opt = ANDI, ins.opc = OpClass::ARITHI; break;
        case 0b110:
            ins.opt = ORI, ins.opc = OpClass::ARITHI; break;
        case 0b100:
            ins.opt = XORI, ins.opc = OpClass::ARITHI; break;
        case 0b001:
            // 24 ~ 20
            ins.imm = ZEXT(ins.ir >> 20, 5);
            if (ins.funct7 == 0b000'0000) ins.opt = SLLI, ins.opc = OpClass::ARITHI;
            break;
        case 0b101:
            // 24 ~ 20
            ins.imm = ZEXT(ins.ir >> 20, 5);
            if (ins.funct7 == 0b000'0000) ins.opt = SRLI, ins.opc = OpClass::ARITHI;
            if (ins.funct7 == 0b010'0000) ins.opt = SRAI, ins.opc = OpClass::ARITHI;
            break;
        case 0b010:
            ins.opt = SLTI, ins.opc = OpClass::ARITHI; break;
        case 0b011:
            ins.opt = SLTIU, ins.opc = OpClass::ARITHI; break;
        };
        break;
    case 0b000'0011:
        switch (ins.funct3) {
        case 0b000:
            ins.opt = LB, ins.opc = OpClass::LOAD; break;
        case 0b001:
            ins.opt = LH, ins.opc = OpClass::LOAD; break;
        case 0b010:
            ins.opt = LW, ins.opc = OpClass::LOAD; break;
        case 0b100:
            ins.opt = LBU, ins.opc = OpClass::LOAD; break;
        case 0b101:
            ins.opt = LHU, ins.opc = OpClass::LOAD; break;
        };
        break;
    case 0b110'0111:
        if (ins.funct3 == 0b000) ins.opt = JALR, ins.opc = OpClass::OTHER;
        break;
    };
}

void Decoder::DecodeS(InsType &ins) {
    GetRs1(ins);
    GetRs2(ins);
    GetFunct3(ins);
    ins.imm = SEXT(Concat((ins.ir >> 7) & 0b1'1111, 5, (ins.ir >> 25)), 12);
    if (ins.opcode == 0b010'0011) {
        switch (ins.funct3) {
        case 0b000:
            ins.opt = SB, ins.opc = OpClass::STORE; break;
        case 0b001:
            ins.opt = SH, ins.opc = OpClass::STORE; break;
        case 0b010:
            ins.opt = SW, ins.opc = OpClass::STORE; break;
        };
    }
}

void Decoder::DecodeB(InsType &ins) {
    GetRs1(ins);
    GetRs2(ins);
    GetFunct3(ins);
    // 11:8 => 4:1  7=>11  30:25 => 10:5 31 => 12
    ins.imm = SEXT(((ins.ir >> 7) & 0b1'1110)
                   | ((ins.ir << 4) & 0b1000'0000'0000)
                   | ((ins.ir >> 20) & 0b0111'1110'0000)
                   | ((ins.ir >> 19) & 0b0001'0000'0000'0000), 13);
    if (ins.opcode == 0b110'0011) {
        switch (ins.funct3) {
        case 0b000:
            ins.opt = BEQ, ins.opc = OpClass::BRANCH; break;
        case 0b001:
            ins.opt = BNE, ins.opc = OpClass::BRANCH; break;
        case 0b100:
            ins.opt = BLT, ins.opc = OpClass::BRANCH; break;
        case 0b110:
            ins.opt = BLTU, ins.opc = OpClass::BRANCH; break;
        case 0b101:
            ins.opt = BGE, ins.opc = OpClass::BRANCH; break;
        case 0b111:
            ins.opt = BGEU, ins.opc = OpClass::BRANCH; break;
        };
    }
}

void Decoder::DecodeU(InsType &ins) {
    GetRd(ins);
    //  31:12 => 31:12
    ins.imm = (ins.ir & 0xFFFFF000);
    switch (ins.opcode) {
    case 0b011'0111:
        ins.opt = LUI, ins.opc = OpClass::OTHER; break;
    case 0b001'0111:
        ins.opt = AUIPC, ins.opc = OpClass::OTHER; break;
    }
}

void Decoder::DecodeJ(InsType &ins) {
    GetRd(ins);
    // 31=>20  30:21=>10:1  20=>11  19:12=>19:12
    ins.imm = SEXT(((ins.ir >> 11) & 0b1'0000'0000'0000'0000'0000)
                   | ((ins.ir >> 20) & 0b0111'1111'1110)
                   | ((ins.ir >> 9) & 0b1000'0000'0000)
                   | ((ins.ir) & 0b1111'1111'0000'0000'0000), 21);
    if (ins.opcode == 0b110'1111) {
        ins.opt = JAL, ins.opc = OpClass::OTHER;
    }
}

void InstructionUnit::FetchDecode(State *cur_state, State *next_state) {
    if (cur_state->wait) {
        return;
    }
    if (cur_state->ins_queue_full) {
        return;
    }
    InsType ins;
    ins.ins_addr = cur_state->pc;
    ins.ir = cur_state->ir;
    decoder.Decode(ins);
    if (ins.opt == LUI) {
        ins.opc = OpClass::ARITHI;
        ins.opt = ADDI;
        ins.rs1 = reName::zero;
        ins.rs2 = -1;
        ins.imm = ins.imm;
    }
    if (ins.opt == AUIPC) {
        ins.opc = OpClass::ARITHI;
        ins.opt = ADDI;
        ins.rs1 = reName::zero;
        ins.rs2 = -1;
        ins.imm = cur_state->pc + ins.imm;
    }
    // PC
    if (ins.opc == OpClass::BRANCH) {
        ins.rd = predictor->GetPrediction(cur_state->pc);
        // We Just Set rd the expect
        if (ins.rd) {
            next_state->pc = cur_state->pc + ins.imm;
        } else {
            next_state->pc = cur_state->pc + 4;
        }
    } else if (ins.opt == JAL) {
        next_state->pc = cur_state->pc + ins.imm;
        ins.opc = OpClass::ARITHI;
        ins.opt = ADDI;
        ins.rs1 = reName::zero;
        ins.rs2 = -1;
        ins.imm = cur_state->pc + 4;
    } else if (ins.opt == JALR) {
        // Just stop read for next pc
        // Wait for JALR Commit
        next_state->wait = true;
    } else if (ins.ir == 0x0ff00513) {
        // halt code !!!
        next_state->wait = true;
    } else {
        next_state->pc = cur_state->pc + 4;
    }
    next_state->ins = {true, ins};
}


void InstructionUnit::Execute(State *cur_state, State *next_state) {
    FetchDecode(cur_state, next_state);
    Issue(cur_state, next_state);
}

void InstructionUnit::Flush(State *cur_state) {
    if (cur_state->clear) {
        ins_queue.clear();
        cur_state->ins_queue_full = ins_queue.full();
    }
    // ins_queue
    if (cur_state->ins.first) {
        if (ins_queue.full()) {
            throw std::runtime_error("Ins queue is full");
        }
        ins_queue.push(cur_state->ins.second);
    }
    cur_state->ins_queue_full = ins_queue.full();
    // 摆烂了 高速内存
    cur_state->ir = mem->ReadWord(cur_state->pc);
}


void InstructionUnit::Issue(State *cur_state, State *next_state) {
    if (ins_queue.empty()) {
        return;
    }
    if (cur_state->rob_full) {
        return;
    }
    auto &front_ins = ins_queue.front();
    RobInter rob_inter{front_ins, RobState::Issue, cur_state->rob_tail_pos, front_ins.rd, 0};
    RsInter rs_inter{front_ins, cur_state->rob_tail_pos};
    LsbInter lsb_inter{front_ins.opc, front_ins.opt, cur_state->rob_tail_pos};
    if (front_ins.opc == OpClass::LOAD || front_ins.opc == OpClass::STORE) {
        // For Load / Store
        if (front_ins.opc == OpClass::LOAD && cur_state->lsb_load_full) return;
        if (front_ins.opc == OpClass::STORE && cur_state->lsb_store_full) return;
        if (cur_state->rs_lsb_full) return;
        // handle vj (rs1)
        if (cur_state->regfile[front_ins.rs1].recorder == -1) {
            // No dependency
            rs_inter.qj = -1;
            rs_inter.vj = cur_state->regfile[front_ins.rs1].data;
        } else {
            // Wait dependency
            rs_inter.qj = cur_state->regfile[front_ins.rs1].recorder;
        }
        // handle vk (rs2 / imm)
        if (front_ins.opc == OpClass::LOAD) {
            // LOAD   rd <== mem[rs1 + imm]
            rs_inter.vk = front_ins.imm;
        } else {
            // STORE mem[rs1 + imm] <== rs2 , vk = rs2
            if (cur_state->regfile[front_ins.rs2].recorder == -1) {
                // No dependency
                rs_inter.qk = -1;
                rs_inter.vk = cur_state->regfile[front_ins.rs2].data;
            } else {
                // Wait dependency
                rs_inter.qk = cur_state->regfile[front_ins.rs2].recorder;
            }
            rs_inter.imm = front_ins.imm;
        }
    } else if (front_ins.opc == OpClass::BRANCH) {
        // For Branch
        if (cur_state->rs_alu_camp_full) return;
        // handle vj (rs1)
        if (cur_state->regfile[front_ins.rs1].recorder == -1) {
            // No dependency
            rs_inter.qj = -1;
            rs_inter.vj = cur_state->regfile[front_ins.rs1].data;
        } else {
            // Wait dependency
            rs_inter.qj = cur_state->regfile[front_ins.rs1].recorder;
        }
        // handle vk (rs2)
        if (cur_state->regfile[front_ins.rs2].recorder == -1) {
            // No dependency
            rs_inter.qk = -1;
            rs_inter.vk = cur_state->regfile[front_ins.rs2].data;
        } else {
            // Wait dependency
            rs_inter.qk = cur_state->regfile[front_ins.rs2].recorder;
        }
        // Set imm(offset)
        rs_inter.imm = front_ins.imm;
    } else if (front_ins.opc == OpClass::ARITHI || front_ins.opc == OpClass::ARITHR) {
        switch (front_ins.opt) {
        case ADD: case ADDI: case SUB:
            if (cur_state->rs_alu_add_full) return;
            break;
        case AND: case ANDI: case OR: case ORI: case XOR: case XORI:
            if (cur_state->rs_alu_logic_full) return;
            break;
        case SLL: case SLLI: case SRL: case SRLI: case SRA: case SRAI:
            if (cur_state->rs_alu_shift_full) return;
            break;
        case SLT: case SLTI: case SLTU: case SLTIU:
            if (cur_state->rs_alu_camp_full) return;
            break;
        default:
            throw std::runtime_error("Unknown opt in Issue ARITHI/ARITHR");
        }
        // handle vj (rs1)
        if (cur_state->regfile[front_ins.rs1].recorder == -1) {
            // No dependency
            rs_inter.qj = -1;
            rs_inter.vj = cur_state->regfile[front_ins.rs1].data;
        } else {
            // Wait dependency
            rs_inter.qj = cur_state->regfile[front_ins.rs1].recorder;
        }
        // handle vk (rs2 / imm)
        if (front_ins.opc == OpClass::ARITHI) {
            rs_inter.qk = -1;
            rs_inter.vk = front_ins.imm;
        } else {
            if (cur_state->regfile[front_ins.rs2].recorder == -1) {
                // No dependency
                rs_inter.qk = -1;
                rs_inter.vk = cur_state->regfile[front_ins.rs2].data;
            } else {
                // Wait dependency
                rs_inter.qk = cur_state->regfile[front_ins.rs2].recorder;
            }
        }
    } else if (front_ins.opt == JALR) {
        // rd = PC + 4
        // PC = (rs1 + imm) data
        if (cur_state->rs_alu_add_full) return;
        // handle vj (rs1)
        if (cur_state->regfile[front_ins.rs1].recorder == -1) {
            // No dependency
            rs_inter.qj = -1;
            rs_inter.vj = cur_state->regfile[front_ins.rs1].data;
        } else {
            // Wait dependency
            rs_inter.qj = cur_state->regfile[front_ins.rs1].recorder;
        }
        // handle vk (imm)
        rs_inter.qk = -1;
        rs_inter.vk = front_ins.imm;
    } else throw std::runtime_error("Unmatch Issue");

    // Change Regfile
    if (front_ins.rd != -1) {
        next_state->regfile[front_ins.rd].recorder = cur_state->rob_tail_pos;
    }

    next_state->query_rob_id1 = rs_inter.qj;
    next_state->query_rob_id2 = rs_inter.qk;
    // send to Rs
    if (front_ins.ir != 0x0ff00513) {
        next_state->rs_inter = {true, rs_inter};
    } else {
        rob_inter.state = RobState::Write;
    }
    // send to ROB
    next_state->rob_inter = {true, rob_inter};
    // send to LSB
    if (front_ins.opc == OpClass::LOAD) {
        next_state->lsb_load_inter = {true, lsb_inter};
    }
    if (front_ins.opc == OpClass::STORE) {
        next_state->lsb_store_inter = {true, lsb_inter};
    }
    ins_queue.pop();
#ifdef DEBUG
    std::cerr << "Issue >>> " << front_ins << std::endl;
    std::cerr << "> RsInter: " << rs_inter << std::endl;
#endif
}

bool Predictor::GetPrediction(AddrType pc) {
    return true;
}

void Predictor::GetFeedBack(AddrType pc, bool real) {
    ;
}

} // namespace jasonfxz