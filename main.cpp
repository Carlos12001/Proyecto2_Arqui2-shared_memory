/*
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
    pe1.add_instruction({InstructionType::INC, 2});
    pe1.add_instruction({InstructionType::INC, 2});
    pe1.add_instruction({InstructionType::STORE, 2, 0x0});
    pe1.add_instruction({InstructionType::LOAD, 0, 0x0});
    pe1.add_instruction({InstructionType::DEC, 0});

    pe2.add_instruction({InstructionType::INC, 3});
    pe2.add_instruction({InstructionType::INC, 3});
    pe2.add_instruction({InstructionType::INC, 3});
    pe2.add_instruction({InstructionType::STORE, 3, 0x1});
    pe2.add_instruction({InstructionType::LOAD, 1, 0x1});
    pe2.add_instruction({InstructionType::DEC, 1});

    std::cout << "Press 'y' to enable stepping" << std::endl;
    char key;
    std::cin >> key;

    if (key == 'y') {
        pe1.enableStepping();
        pe2.enableStepping();
    }

    // Start both PEs
    pe1.start();
    pe2.start();

    bool running_flag = true;

    while (running_flag) {
        std::cout << "Press 's' to step or any other key to exit: ";
        std::cin >> key;
        if (key == 's') {
            cond.notify_all();
        } else {
            running_flag = false;
            pe1.stop();
            pe2.stop();
        }
    }

    return 0;
}
*/



/*
#include "shared_memory.h"
#include <iostream>

int main() {
    SharedMemory sharedMemory(256);

    sharedMemory.write(10, 123456789);
    std::cout << "Value at address 10: " << sharedMemory.read(10) << std::endl;

    return 0;
}
*/

#include <iostream>
#include "cache.h"
#include "bus.h"

int main() {
    // Crear el bus
    Bus systemBus;

    // Crear cachés y registrarlas en el bus
    Cache cache1(8); // Supongamos 8 bloques de caché para simplificar
    Cache cache2(8);
    systemBus.registerCache(&cache1);
    systemBus.registerCache(&cache2);

    // Simulación de acceso a la misma dirección de memoria por diferentes cachés
    int addr = 5; // Dirección de memoria de prueba

    // Procesador 1 escribe en la dirección
    cache1.store(addr, 100);
    systemBus.sendBusRdX(addr);

    // Procesador 2 intenta leer la misma dirección
    std::cout << "Procesador 2 lee la dirección " << addr << ": " << cache2.load(addr) << std::endl;
    systemBus.sendBusRd(addr);

    // Verificar y mostrar los estados de la caché
    // (Esto requeriría funciones de acceso que no hemos definido, es solo un ejemplo conceptual)
    // Ejemplo: cache1.printCacheState();
    // cache2.printCacheState();

    return 0;
}
