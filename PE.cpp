#include "PE.h"
#include <iostream>

/*
ProcessingElement::ProcessingElement(int id, std::mutex* mtx, std::condition_variable* cv) : pc(0), pe_id(id), mutex(mtx), cond(cv), registers({0, 0, 0, 0}), cache(64) {}

void ProcessingElement::add_instruction(const Instruction& instr) {
    instructions.push_back(instr);
}

void ProcessingElement::start() {
    running = true;  // Ensure the PE starts running initially
    if(steppingOn){
        std::cout << "step" << std::endl;
        thread = std::thread(&ProcessingElement::step, this);
    }else{
        std::cout << "runnig" << std::endl;
        thread = std::thread(&ProcessingElement::execute, this);
    }
}

void ProcessingElement::stop() {
    running = false;  // Signal the PE to stop execution
    cond->notify_one();     // Notify the PE to exit
    if (thread.joinable()) {
        thread.join();
    }
}

void ProcessingElement::step() {
    std::unique_lock<std::mutex> lock(*mutex);
    while (running && pc < instructions.size()) {
        cond->wait(lock);  // Wait until a signal to step or run the next instruction
        if (!running) break;
        handle_instruction(instructions[pc]);
        pc++;
    }
}

void ProcessingElement::enableStepping() {
    steppingOn = true;
}

void ProcessingElement::execute() {
    std::unique_lock<std::mutex> lock(*mutex); // Lock the mutex before waiting
    while (running && !instructions.empty()) {
        std::cout << "Si entró" << std::endl;
        auto instr = instructions[pc];
        handle_instruction(instr);
        pc++;
        if (pc >= static_cast<int>(instructions.size())) {
            running = false;  // Reached end of program
        }
    }
}

void ProcessingElement::handle_instruction(const Instruction& instr) {
    switch (instr.type) {
        case InstructionType::LOAD:
            std::cout << "PE " << pe_id << ": LOAD at address " << instr.addr << std::endl;
            registers[instr.reg_index] = cache.load(instr.addr);
            break;
        case InstructionType::STORE:
            cache.store(instr.addr, registers[instr.reg_index]);
            std::cout << "PE " << pe_id << ": STORE at address " << instr.addr << std::endl;
            break;
        case InstructionType::INC:
            std::cout << "PE " << pe_id << ": INC register index " << instr.reg_index << std::endl;
            registers[instr.reg_index]++;
            break;
        case InstructionType::DEC:
            std::cout << "PE " << pe_id << ": DEC register index " << instr.reg_index << std::endl;
            registers[instr.reg_index]--;
            break;
        case InstructionType::JNZ: {
            bool cond = (pc % 2 == 0); // Example condition
            if (!cond) pc = instructions.size() - 1; // Jump to end of program
            break;
        }
    }

    printRegisterStatus();
}

void ProcessingElement::printRegisterStatus() {
    std::cout << "PE " << pe_id << ": Register Status" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "Register R" << i << ": " << registers[i] << std::endl;
    }
}
*/