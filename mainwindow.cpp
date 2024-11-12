// mainwindow.cpp
#include "mainwindow.h"

#include <QHeaderView>  // Agrega esta línea

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      cache0(8, &sharedMemory),
      cache1(8, &sharedMemory),
      sharedMemory(256),
      busCycles(0) {
  setupUI();

  // Registrar las cachés en el bus
  systemBus.registerCache(&cache0);
  systemBus.registerCache(&cache1);

  // Conectar las señales de los botones con los slots
  connect(readButton, &QPushButton::clicked, this,
          &MainWindow::on_readButton_clicked);
  connect(writeButton, &QPushButton::clicked, this,
          &MainWindow::on_writeButton_clicked);
  connect(resetButton, &QPushButton::clicked, this,
          &MainWindow::on_resetButton_clicked);
  connect(helpButton, &QPushButton::clicked, this,
          &MainWindow::on_helpButton_clicked);

  // Configurar el temporizador para actualizar la interfaz periódicamente
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &MainWindow::updateDisplays);
  timer->start(500);  // Actualizar cada 500 ms

  // Actualizar la interfaz inicialmente
  updateDisplays();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
  // Establecer el título de la ventana
  setWindowTitle("MESI Protocol Simulator");

  // Crear los widgets
  titleLabel = new QLabel("MESI Protocol");
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(16);
  titleLabel->setFont(titleFont);

  instructionLabel = new QLabel(
      "Use los botones para ejecutar operaciones en los CPUs. Mantenga "
      "presionada la tecla CTRL para seleccionar múltiples acciones.");

  busCycleLabel = new QLabel("Bus Cycles: 0");

  // Tabla de memoria compartida
  memoryTableWidget = new QTableWidget(16, 2);  // 16 filas, 2 columnas
  memoryTableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "Address" << "Data");
  memoryTableWidget->verticalHeader()->setVisible(false);
  memoryTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // Tablas de cachés
  cache0TableWidget = new QTableWidget(8, 3);  // 8 bloques, 3 columnas
  cache0TableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "Tag" << "Data" << "State");
  cache0TableWidget->verticalHeader()->setVisible(false);
  cache0TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

  cache1TableWidget = new QTableWidget(8, 3);
  cache1TableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "Tag" << "Data" << "State");
  cache1TableWidget->verticalHeader()->setVisible(false);
  cache1TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // Campos de dirección y valor
  addressLineEdit = new QLineEdit();
  addressLineEdit->setPlaceholderText("Address");
  valueLineEdit = new QLineEdit();
  valueLineEdit->setPlaceholderText("Value");

  // Botones
  readButton = new QPushButton("Read");
  writeButton = new QPushButton("Write");
  resetButton = new QPushButton("Reset");
  helpButton = new QPushButton("Help");

  // Layout principal
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  // Añadir los widgets al layout
  mainLayout->addWidget(titleLabel);
  mainLayout->addWidget(instructionLabel);

  // Layout de memoria y bus cycles
  QHBoxLayout *memoryLayout = new QHBoxLayout();
  memoryLayout->addWidget(new QLabel("MEMORY"));
  memoryLayout->addWidget(memoryTableWidget);
  memoryLayout->addStretch();
  memoryLayout->addWidget(busCycleLabel);

  mainLayout->addLayout(memoryLayout);

  // Línea horizontal para representar el bus
  QFrame *line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(line);

  // Layout de cachés
  QHBoxLayout *cacheLayout = new QHBoxLayout();
  QVBoxLayout *cache0Layout = new QVBoxLayout();
  cache0Layout->addWidget(new QLabel("CACHE 0"));
  cache0Layout->addWidget(cache0TableWidget);

  QVBoxLayout *cache1Layout = new QVBoxLayout();
  cache1Layout->addWidget(new QLabel("CACHE 1"));
  cache1Layout->addWidget(cache1TableWidget);

  cacheLayout->addLayout(cache0Layout);
  cacheLayout->addLayout(cache1Layout);

  mainLayout->addLayout(cacheLayout);

  // Línea horizontal para representar el bus
  QFrame *line2 = new QFrame();
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);
  mainLayout->addWidget(line2);

  // Layout de CPUs y botones
  QHBoxLayout *cpuLayout = new QHBoxLayout();

  // CPU 0
  QVBoxLayout *cpu0Layout = new QVBoxLayout();
  cpu0Layout->addWidget(new QLabel("CPU 0"));
  cpu0Layout->addWidget(addressLineEdit);
  cpu0Layout->addWidget(valueLineEdit);
  cpu0Layout->addWidget(readButton);
  cpu0Layout->addWidget(writeButton);

  // CPU 1 (puedes agregar botones adicionales si lo deseas)
  QVBoxLayout *cpu1Layout = new QVBoxLayout();
  cpu1Layout->addWidget(new QLabel("CPU 1"));
  // Agrega botones y campos si quieres que CPU 1 sea interactivo

  cpuLayout->addLayout(cpu0Layout);
  cpuLayout->addLayout(cpu1Layout);

  // Controles de ejecución
  QVBoxLayout *controlLayout = new QVBoxLayout();
  controlLayout->addWidget(resetButton);
  controlLayout->addWidget(helpButton);

  cpuLayout->addLayout(controlLayout);

  mainLayout->addLayout(cpuLayout);

  // Establecer el widget central
  setCentralWidget(centralWidget);
}

void MainWindow::on_readButton_clicked() {
  int addr = addressLineEdit->text().toInt();

  // Procesador 0 lee desde la dirección
  uint64_t data = cache0.load(addr, &systemBus);

  // Incrementar ciclos de bus si se generó una transacción
  busCycles++;

  // Actualizar la interfaz
  updateDisplays();
}

void MainWindow::on_writeButton_clicked() {
  int addr = addressLineEdit->text().toInt();
  uint64_t value = valueLineEdit->text().toULongLong();

  // Procesador 0 escribe en la dirección
  cache0.store(addr, value, &systemBus);

  // Incrementar ciclos de bus
  busCycles++;

  // Actualizar la interfaz
  updateDisplays();
}

// mainwindow.cpp
void MainWindow::on_resetButton_clicked() {
  // Reiniciar los objetos de simulación
  sharedMemory.reset();
  cache0.reset();
  cache1.reset();
  busCycles = 0;

  // Actualizar la interfaz
  updateDisplays();
}

void MainWindow::on_helpButton_clicked() {
  QMessageBox::information(
      this, "Help",
      "Instrucciones de uso:\n\n- Ingrese la dirección en el campo "
      "'Address'.\n- Ingrese el valor en el campo 'Value' (para escritura).\n- "
      "Use 'Read' para leer desde la dirección especificada.\n- Use 'Write' "
      "para escribir el valor en la dirección especificada.\n- Presione "
      "'Reset' para reiniciar la simulación.\n\nPuede realizar operaciones "
      "simultáneas en los CPUs utilizando los botones correspondientes.");
}

void MainWindow::updateDisplays() {
  updateMemoryDisplay();
  updateCacheDisplay();
  updateBusCycleCount();
}

void MainWindow::updateMemoryDisplay() {
  int numRows = memoryTableWidget->rowCount();

  for (int addr = 0; addr < numRows; ++addr) {
    uint64_t value = sharedMemory.read(addr);

    QTableWidgetItem *addressItem = new QTableWidgetItem(QString::number(addr));
    addressItem->setTextAlignment(Qt::AlignCenter);
    memoryTableWidget->setItem(addr, 0, addressItem);

    QTableWidgetItem *dataItem = new QTableWidgetItem(QString::number(value));
    dataItem->setTextAlignment(Qt::AlignCenter);
    memoryTableWidget->setItem(addr, 1, dataItem);
  }
}

void MainWindow::updateCacheDisplay() {
  // Actualizar CACHE 0
  for (int i = 0; i < cache0.getNumBlocks(); ++i) {
    CacheBlock block = cache0.getBlock(i);

    QTableWidgetItem *tagItem =
        new QTableWidgetItem(QString::number(block.tag));
    tagItem->setTextAlignment(Qt::AlignCenter);
    cache0TableWidget->setItem(i, 0, tagItem);

    QTableWidgetItem *dataItem =
        new QTableWidgetItem(QString::number(block.data));
    dataItem->setTextAlignment(Qt::AlignCenter);
    cache0TableWidget->setItem(i, 1, dataItem);

    QString stateStr;
    switch (block.state) {
      case MESIState::Modified:
        stateStr = "M";
        break;
      case MESIState::Exclusive:
        stateStr = "E";
        break;
      case MESIState::Shared:
        stateStr = "S";
        break;
      case MESIState::Invalid:
        stateStr = "I";
        break;
    }

    QTableWidgetItem *stateItem = new QTableWidgetItem(stateStr);
    stateItem->setTextAlignment(Qt::AlignCenter);
    cache0TableWidget->setItem(i, 2, stateItem);
  }

  // Actualizar CACHE 1
  for (int i = 0; i < cache1.getNumBlocks(); ++i) {
    CacheBlock block = cache1.getBlock(i);

    QTableWidgetItem *tagItem =
        new QTableWidgetItem(QString::number(block.tag));
    tagItem->setTextAlignment(Qt::AlignCenter);
    cache1TableWidget->setItem(i, 0, tagItem);

    QTableWidgetItem *dataItem =
        new QTableWidgetItem(QString::number(block.data));
    dataItem->setTextAlignment(Qt::AlignCenter);
    cache1TableWidget->setItem(i, 1, dataItem);

    QString stateStr;
    switch (block.state) {
      case MESIState::Modified:
        stateStr = "M";
        break;
      case MESIState::Exclusive:
        stateStr = "E";
        break;
      case MESIState::Shared:
        stateStr = "S";
        break;
      case MESIState::Invalid:
        stateStr = "I";
        break;
    }

    QTableWidgetItem *stateItem = new QTableWidgetItem(stateStr);
    stateItem->setTextAlignment(Qt::AlignCenter);
    cache1TableWidget->setItem(i, 2, stateItem);
  }
}

void MainWindow::updateBusCycleCount() {
  busCycleLabel->setText(QString("Bus Cycles: %1").arg(busCycles));
}
