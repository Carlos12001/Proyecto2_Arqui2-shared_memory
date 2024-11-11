cmake_minimum_required(VERSION 3.5)
project(shared_memory)

set(CMAKE_CXX_STANDARD 17)

# Encuentra las bibliotecas de Qt
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Agrega todos los archivos fuente en el directorio actual
set(SOURCES
    main.cpp
    bus.cpp
    cache.cpp
    PE.cpp
    shared_memory.cpp
)

# Define el ejecutable y enlaza las bibliotecas de Qt
add_executable(qt_project ${SOURCES})
target_link_libraries(qt_project Qt5::Widgets)
