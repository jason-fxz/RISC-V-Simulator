
#include <iostream>
#include <fstream>
#include "naive_simulator.h"
#include "simulator.h"
#include "utils/utils.h"
#include <cassert>

int duipai() {
    jasonfxz::NSimulator nsim;
    jasonfxz::Simulator sim;
    const char *inputFileName = "./testcases/bulgarian.data";
    std::ifstream inputFile(inputFileName, std::ios::in);
    if (!inputFile) {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }
    sim.Init(inputFile);
    inputFile.close();
    inputFile.open(inputFileName, std::ios::in);
    nsim.Init(inputFile);
    inputFile.close();
    jasonfxz::DebugRecord last;
    int step_count = 0;
    while (true) {
        ++step_count;
        jasonfxz::DebugRecord ans, out;
        // if (step_count == 4952) {
        //     sim.enable_debug = true;
        //     nsim.enable_debug = true;
        // }
        bool nflag = nsim.Step(ans);
        bool flag = sim.Step(out);
        assert(nflag == flag);
        if (nflag == false) break;
        if (!(ans == out)) {
            std::cerr << "Different output" << std::endl;
            ans.Print();
            std::cerr << "vs" << std::endl;
            out.Print();
            std::cerr << "LAST OUTPUT" << std::endl;
            last.Print();
            std::cerr << "At step " << step_count << std::endl;
            throw "Error";
        }
        last = ans;
    }
    return 0;
}

void omain() {
    jasonfxz::Simulator sim;
    sim.Init(std::cin);
    int ans = sim.Run();
    std::cout << ans << std::endl;
}

int main() {
    // return duipai(); 
    omain();
    return 0;
}