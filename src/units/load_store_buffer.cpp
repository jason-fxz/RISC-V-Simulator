#include "units/load_store_buffer.h"
#include "circuits/bus.h"
#include "config/types.h"
#include "config/constant.h"
#include "simulator.h"
#include <cassert>
#include <climits>
#include <stdexcept>

namespace jasonfxz {



void LoadStoreBuffer::Flush(State *cur_state) {
    if (cur_state->clear) {
        load_queue.clear();
        // the storing should have be done
        store_queue.clear();
        assert(store_counter == 0);
        load_counter = store_counter = 0;
        cur_state->lsb_load_full = load_queue.full();
        cur_state->lsb_store_full = store_queue.full();
    }
    if (cur_state->lsb_load_inter.first) {
        if (!load_queue.push({cur_state->clock, cur_state->lsb_load_inter.second})) {
            throw std::runtime_error("Load queue full");
        }
    }
    if (cur_state->lsb_store_inter.first) {
        if (!store_queue.push({cur_state->clock, cur_state->lsb_store_inter.second})) {
            throw std::runtime_error("Store queue full");
        }
    }
    // Set the full flag
    cur_state->lsb_load_full = load_queue.full();
    cur_state->lsb_store_full = store_queue.full();
    // CD BUS
    for (const auto &it : cd_bus->e) if (it.first) {
            const auto &info = it.second;
            if (info.type == BusType::GetAddr) {
                // Check CDB for Load and Store address
                for (auto &it : load_queue) {
                    if (it.second.rob_pos == info.pos) {
                        it.second.addr_ready = 1;
                        it.second.addr = info.data;
                    }
                }
                for (auto &it : store_queue) {
                    if (it.second.rob_pos == info.pos) {
                        it.second.addr_ready = 1;
                        it.second.addr = info.data;
                    }
                }
            } else if (info.type == BusType::CommitMem) {
                // Store commit (Give the data)
                assert(info.pos == store_queue.front().second.rob_pos);
                assert(store_queue.front().second.addr_ready);
                store_enable = true;
                store_data = info.data; // Get Data (the data to store)
            }
        }
    if (store_queue.empty()) {
        load_enable_level = INT_MAX;
    } else {
        load_enable_level = store_queue.front().first;
    }
}


void LoadStoreBuffer::Execute(State *cur_state, State *next_state) {
    if (load_counter == 0) { // load is available
        // Load
        if (!load_queue.empty()) {
            auto &front = load_queue.front();
            if (front.second.addr_ready && front.first <= load_enable_level) {
                // Loading
                load_counter = 1;
            }
        }
    } else {
        if (load_counter == load_latency) {
            // Load
            auto &front = load_queue.front();
            BusInter bus_inter;
            bus_inter.type = BusType::WriteBack;
            bus_inter.pos = front.second.rob_pos;
            switch (front.second.opt) {
            case LB: bus_inter.data = mem->ReadByte(front.second.addr); break;
            case LH: bus_inter.data = mem->ReadHalf(front.second.addr); break;
            case LW: bus_inter.data = mem->ReadWord(front.second.addr); break;
            case LBU: bus_inter.data = mem->ReadByteU(front.second.addr); break;
            case LHU: bus_inter.data = mem->ReadHalfU(front.second.addr); break;
            default: throw std::runtime_error("Invalid load type");
            }
            load_queue.pop();
            if (!cd_bus->e.insert(bus_inter)) {
                throw std::runtime_error("Cd bus full");
            }
            load_counter = 0;
        } else {
            load_counter++;
        }
    }
    if (store_counter == 0) { // store is available
        // Store
        if (store_enable) {
            // Storing
            store_counter = 1;
            store_enable = false;
        }
    } else {
        if (store_counter == store_latency) {
            // Store
            switch (store_queue.front().second.opt) {
            case SB: mem->WriteByte(store_queue.front().second.addr, store_data); break;
            case SH: mem->WriteHalf(store_queue.front().second.addr, store_data); break;
            case SW: mem->WriteWord(store_queue.front().second.addr, store_data); break;
            default: throw std::runtime_error("Invalid store type");
            }
            BusInter bus_inter{BusType::StoreSuccess, 0, store_queue.front().second.rob_pos};
            if (!cd_bus->e.insert(bus_inter)) {
                throw std::runtime_error("Cd bus full");
            }
            store_queue.pop();
            store_counter = 0;
        } else {
            store_counter++;
        }
    }
}

} // namespace jasonfxz