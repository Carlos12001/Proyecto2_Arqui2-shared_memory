cmake_minimum_required(VERSION 3.5)
project(shared_memory)

set(CMAKE_CXX_STANDARD 17)

# Encuentra las bibliotecas de Qt
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Incluye los archivos de cabecera con Q_OBJECT
set(HEADERS
    mainwindow.h
)

# Agrega todos los archivos fuente
set(SOURCES
    main.cpp
    bus.cpp
    cache.cpp
    PE.cpp
    shared_memory.cpp
    mainwindow.cpp
)

# Incluye los archivos de cabecera para moc
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

# Define el ejecutable y enlaza las bibliotecas de Qt
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
