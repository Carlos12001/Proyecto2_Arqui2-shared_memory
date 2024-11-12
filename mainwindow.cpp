// mainwindow.cpp

#include "mainwindow.h"

#include <QHeaderView>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), sharedMemory(256), busCycles(0) {
  setupUI();

  // Crear cachés y registrarlas en el bus
  for (int i = 0; i < NUM_CPUS; ++i) {
    Cache *cache = new Cache(8, &sharedMemory);
    caches.push_back(cache);
    systemBus.registerCache(cache);
  }

  // Conectar los botones de CPUs
  for (int cpuId = 0; cpuId < NUM_CPUS; ++cpuId) {
    for (int addr = 0; addr < 4; ++addr) {
      connect(cpuReadButtons[cpuId * 4 + addr], &QPushButton::clicked,
              [=]() { on_cpuReadButton_clicked(cpuId, addr); });
      connect(cpuWriteButtons[cpuId * 4 + addr], &QPushButton::clicked,
              [=]() { on_cpuWriteButton_clicked(cpuId, addr); });
    }
  }

  // Conectar botones de control
  connect(resetButton, &QPushButton::clicked, this,
          &MainWindow::on_resetButton_clicked);
  connect(helpButton, &QPushButton::clicked, this,
          &MainWindow::on_helpButton_clicked);

  // Configurar el temporizador para actualizaciones periódicas
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &MainWindow::updateDisplays);
  timer->start(500);

  // Actualizar la interfaz inicialmente
  updateDisplays();
}

MainWindow::~MainWindow() {
  // Liberar memoria
  for (auto cache : caches) {
    delete cache;
  }
}

void MainWindow::setupUI() {
  setWindowTitle("MESI Protocol Simulator");

  centralWidget = new QWidget(this);
  mainLayout = new QVBoxLayout(centralWidget);

  // Título y instrucciones
  titleLabel = new QLabel("MESI Protocol");
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(16);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);

  instructionLabel = new QLabel(
      "Like real hardware, the CPUs can operate in parallel. Try pressing a "
      "button on different CPUs \"simultaneously\". Alternatively, select "
      "buttons on different CPUs with the CTRL key and click on the last "
      "button without CTRL to start simultaneous transactions.");

  mainLayout->addWidget(titleLabel);
  mainLayout->addWidget(instructionLabel);

  // Crear representaciones gráficas
  createMemoryDisplay();
  createCacheDisplays();
  createCPUDisplays();
  createControlButtons();

  setCentralWidget(centralWidget);
}

void MainWindow::createMemoryDisplay() {
  QGroupBox *memoryGroup = new QGroupBox("Shared Memory");
  QVBoxLayout *memoryLayout = new QVBoxLayout(memoryGroup);

  // Tabla de memoria
  memoryTableWidget = new QTableWidget(4, 2);
  memoryTableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "Address" << "Data");
  memoryTableWidget->verticalHeader()->setVisible(false);
  memoryTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  memoryTableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  memoryTableWidget->setFixedHeight(150);

  memoryLayout->addWidget(memoryTableWidget);

  mainLayout->addWidget(memoryGroup);
}

void MainWindow::createCacheDisplays() {
  QGroupBox *cachesGroup = new QGroupBox("Caches");
  QHBoxLayout *cachesLayout = new QHBoxLayout(cachesGroup);

  for (int i = 0; i < NUM_CPUS; ++i) {
    QGroupBox *cacheGroup = new QGroupBox(QString("Cache %1").arg(i));
    QVBoxLayout *cacheLayout = new QVBoxLayout(cacheGroup);

    QTableWidget *cacheTable = new QTableWidget(2, 3);
    cacheTable->setHorizontalHeaderLabels(QStringList()
                                          << "Tag" << "Data" << "State");
    cacheTable->verticalHeader()->setVisible(false);
    cacheTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cacheTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    cacheTable->setFixedHeight(150);

    cacheLayout->addWidget(cacheTable);
    cachesLayout->addWidget(cacheGroup);

    cacheTableWidgets.push_back(cacheTable);
  }

  // Añadir una barra de desplazamiento si hay muchas cachés
  QScrollArea *scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(cachesGroup);

  mainLayout->addWidget(scrollArea);
}

void MainWindow::createCPUDisplays() {
  QGroupBox *cpusGroup = new QGroupBox("CPUs");
  QHBoxLayout *cpusLayout = new QHBoxLayout(cpusGroup);

  for (int cpuId = 0; cpuId < NUM_CPUS; ++cpuId) {
    QGroupBox *cpuGroup = new QGroupBox(QString("CPU %1").arg(cpuId));
    QVBoxLayout *cpuLayout = new QVBoxLayout(cpuGroup);

    // Crear botones de lectura y escritura para direcciones 0 a 3
    for (int addr = 0; addr < 4; ++addr) {
      QPushButton *readButton = new QPushButton(QString("Read a%1").arg(addr));
      readButton->setFixedSize(100, 30);
      cpuLayout->addWidget(readButton);

      QPushButton *writeButton =
          new QPushButton(QString("Write a%1").arg(addr));
      writeButton->setFixedSize(100, 30);
      cpuLayout->addWidget(writeButton);

      cpuReadButtons.push_back(readButton);
      cpuWriteButtons.push_back(writeButton);
    }

    cpusLayout->addWidget(cpuGroup);
  }

  // Añadir una barra de desplazamiento si hay muchas CPUs
  QScrollArea *scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(cpusGroup);

  mainLayout->addWidget(scrollArea);
}

void MainWindow::createControlButtons() {
  QHBoxLayout *controlLayout = new QHBoxLayout;

  resetButton = new QPushButton("Reset");
  helpButton = new QPushButton("Help");

  controlLayout->addWidget(resetButton);
  controlLayout->addWidget(helpButton);

  busCycleLabel = new QLabel("Bus Cycles: 0");
  busCycleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
  controlLayout->addWidget(busCycleLabel);

  mainLayout->addLayout(controlLayout);
}

void MainWindow::on_cpuReadButton_clicked(int cpuId, int address) {
  // CPU realiza una operación de lectura
  uint64_t data = caches[cpuId]->load(address, &systemBus);

  // Incrementar ciclos de bus si se generó una transacción
  busCycles++;

  // Actualizar la interfaz
  updateDisplays();
}

void MainWindow::on_cpuWriteButton_clicked(int cpuId, int address) {
  // CPU realiza una operación de escritura
  uint64_t value =
      busCycles;  // Valor de ejemplo, puedes modificarlo según tu lógica
  caches[cpuId]->store(address, value, &systemBus);

  // Incrementar ciclos de bus
  busCycles++;

  // Actualizar la interfaz
  updateDisplays();
}

void MainWindow::on_resetButton_clicked() { resetSimulation(); }

void MainWindow::on_helpButton_clicked() {
  QMessageBox::information(
      this, "Help",
      "Instructions:\n\n- Click on 'Read' or 'Write' buttons to perform "
      "operations.\n- The simulation will show how the MESI protocol handles "
      "cache coherence.\n- 'Reset' button will restart the simulation.");
}

void MainWindow::updateDisplays() {
  updateMemoryDisplay();
  updateCacheDisplays();
  updateBusCycleCount();
}

void MainWindow::updateMemoryDisplay() {
  for (int addr = 0; addr < 4; ++addr) {
    uint64_t value = sharedMemory.read(addr);

    QTableWidgetItem *addressItem = new QTableWidgetItem(QString::number(addr));
    addressItem->setTextAlignment(Qt::AlignCenter);
    memoryTableWidget->setItem(addr, 0, addressItem);

    QTableWidgetItem *dataItem = new QTableWidgetItem(QString::number(value));
    dataItem->setTextAlignment(Qt::AlignCenter);
    memoryTableWidget->setItem(addr, 1, dataItem);
  }
}

void MainWindow::updateCacheDisplays() {
  for (int cpuId = 0; cpuId < NUM_CPUS; ++cpuId) {
    QTableWidget *cacheTable = cacheTableWidgets[cpuId];
    Cache *cache = caches[cpuId];

    for (int i = 0; i < cache->getNumBlocks(); ++i) {
      CacheBlock block = cache->getBlock(i);

      QTableWidgetItem *tagItem =
          new QTableWidgetItem(block.valid ? QString::number(block.tag) : "-");
      tagItem->setTextAlignment(Qt::AlignCenter);
      cacheTable->setItem(i, 0, tagItem);

      QTableWidgetItem *dataItem =
          new QTableWidgetItem(block.valid ? QString::number(block.data) : "-");
      dataItem->setTextAlignment(Qt::AlignCenter);
      cacheTable->setItem(i, 1, dataItem);

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

      QTableWidgetItem *stateItem =
          new QTableWidgetItem(block.valid ? stateStr : "-");
      stateItem->setTextAlignment(Qt::AlignCenter);
      cacheTable->setItem(i, 2, stateItem);
    }
  }
}

void MainWindow::updateBusCycleCount() {
  busCycleLabel->setText(QString("Bus Cycles: %1").arg(busCycles));
}

void MainWindow::resetSimulation() {
  // Reiniciar la memoria compartida y los cachés
  sharedMemory.reset();
  for (auto cache : caches) {
    cache->reset();
  }
  busCycles = 0;

  // Actualizar la interfaz
  updateDisplays();
}
