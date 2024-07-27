
#include <iostream>

#ifdef NAIVE
#include "naive_simulator.h"
#else
#include "simulator.h"
#endif

int main() {
    jasonfxz::Simulator sim;
    sim.Init();
    std::cout << (int)sim.Run() << std::endl;
}