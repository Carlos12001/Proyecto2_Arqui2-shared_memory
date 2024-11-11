#ifndef PE_H
#define PE_H

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "cache.h"

// Define Instruction enum for different types of instructions.
enum class InstructionType { LOAD, STORE, INC, DEC, JNZ };

struct Instruction {
    InstructionType type;
    int reg_index;  // Register index
    int addr;       // Address used in LOAD and STORE
    std::string label;  // Label used by JNZ

    Instruction(InstructionType t = InstructionType::LOAD, int r = -1, int a = -1, const std::string& l = "") : type(t), reg_index(r), addr(a), label(l) {}
};

class ProcessingElement {
public:
    ProcessingElement(int id, std::mutex* mtx, std::condition_variable* cv);
    
    void add_instruction(const Instruction& instr);
    void start();
    void stop();
    void step();
    void enableStepping();

private:
    int pc;                   // Program counter
    int pe_id;
    std::vector<Instruction> instructions;
    std::vector<int> registers;

    std::thread thread;
    bool running = false;      // Whether the PE should keep executing
    bool steppingOn = false;

    std::mutex* mutex;
    std::condition_variable* cond;

    Cache cache;

    void execute();
    void handle_instruction(const Instruction& instr);
    void printRegisterStatus();
};

#endif
