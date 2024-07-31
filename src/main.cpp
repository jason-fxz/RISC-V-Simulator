
#include <cstdio>
#include <iostream>
#include <fstream>
#include "naive_simulator.h"
#include "simulator.h"


int main() {
    jasonfxz::NSimulator nsim;
    std::cerr << "Using advanced simulator" << std::endl;
    jasonfxz::Simulator sim;

    const char *inputFileName = "input.txt";

    std::ifstream inputFile(inputFileName, std::ios::in);
    if (!inputFile) {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }
    nsim.Init(inputFile);
    inputFile.seekg(0, std::ios::beg);
    inputFile.clear();
    sim.Init(inputFile);
    
    
    return 0;
}