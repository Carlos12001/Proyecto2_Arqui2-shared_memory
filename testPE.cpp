#include <iostream>
#include "PE.h"
#include <mutex>
#include <condition_variable>

int main() {
    std::mutex mutex;
    std::condition_variable cond;

    ProcessingElement pe1(1, &mutex, &cond);
    ProcessingElement pe2(2, &mutex, &cond);

    // Add instructions for PE1 and PE2
    pe1.add_instruction({InstructionType::LOAD, 0, 5});
    pe1.add_instruction({InstructionType::INC, 1});
    pe1.add_instruction({InstructionType::STORE, 2});
    pe1.add_instruction({InstructionType::DEC, 3});

    pe2.add_instruction({InstructionType::LOAD, 0, 5});
    pe2.add_instruction({InstructionType::INC, 3});
    pe2.add_instruction({InstructionType::STORE, 2});
    pe2.add_instruction({InstructionType::DEC, 1});

    // Start both PEs
    pe1.start();
    pe2.start();

    bool running_flag = true;
    char key;

    while (running_flag) {
        std::cout << "Press 's' to step or any other key to exit: ";
        std::cin >> key;
        if (key == 's') {
            pe1.step();
            pe2.step();
        } else {
            running_flag = false;
            pe1.stop();
            pe2.stop();
        }
    }

    return 0;
}