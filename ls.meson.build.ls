project('simulador_multiprocesador', 'cpp',
        default_options : ['cpp_std=c++20'],  
        version : '1.0.0')

sources = files('main.cpp', 'shared_memory.cpp', 'PE.cpp', 'cache.cpp', 'bus.cpp')

executable('simulador_multiprocesador', sources)
