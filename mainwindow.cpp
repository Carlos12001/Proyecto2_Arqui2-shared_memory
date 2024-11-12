#include "mainwindow.h"

#include <QGraphicsLineItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsTextItem>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), sharedMemory(256), busCycles(0) {
  setupUI();

  // Crear cachés y registrarlas en el bus
  for (int i = 0; i < 3; ++i) {
    Cache *cache = new Cache(8, &sharedMemory);
    caches.push_back(cache);
    systemBus.registerCache(cache);
  }

  // Conectar los botones de CPUs
  for (int cpuId = 0; cpuId < 3; ++cpuId) {
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

  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

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

  // Crear la vista gráfica para los buses y otros elementos visuales
  graphicsView = new QGraphicsView();
  scene = new QGraphicsScene(0, 0, 1024, 640, this);
  graphicsView->setScene(scene);
  mainLayout->addWidget(graphicsView);

  // Crear representaciones gráficas
  createMemoryDisplay();
  createBusDisplay();
  createCacheDisplays();
  createCPUDisplays();
  createControlButtons();

  setCentralWidget(centralWidget);
}

void MainWindow::createMemoryDisplay() {
  // Crear un rectángulo para la memoria
  QGraphicsRectItem *memoryRect =
      scene->addRect(412, 80, 200, 100, QPen(Qt::black), QBrush(Qt::lightGray));
  QGraphicsTextItem *memoryLabel = scene->addText("MEMORY");
  memoryLabel->setPos(472, 50);

  // Tabla de memoria
  memoryTableWidget = new QTableWidget(4, 2);
  memoryTableWidget->setHorizontalHeaderLabels(QStringList() << "Address"
                                                             << "Data");
  memoryTableWidget->verticalHeader()->setVisible(false);
  memoryTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  memoryTableWidget->setFixedSize(200, 100);

  // Agregar la tabla a la escena
  QGraphicsProxyWidget *memoryTableProxy = scene->addWidget(memoryTableWidget);
  memoryTableProxy->setPos(412, 80);
}

void MainWindow::createBusDisplay() {
  // Dibujar líneas que representan los buses
  QPen addressBusPen(Qt::blue, 5);
  scene->addLine(50, 290, 974, 290, addressBusPen);
  QGraphicsTextItem *addressBusLabel = scene->addText("Address Bus");
  addressBusLabel->setPos(60, 260);

  QPen dataBusPen(Qt::red, 5);
  scene->addLine(50, 330, 974, 330, dataBusPen);
  QGraphicsTextItem *dataBusLabel = scene->addText("Data Bus");
  dataBusLabel->setPos(60, 340);

  QPen sharedBusPen(Qt::magenta, 3);
  scene->addLine(50, 310, 974, 310, sharedBusPen);
  QGraphicsTextItem *sharedBusLabel = scene->addText("Shared Bus");
  sharedBusLabel->setPos(60, 280);
}

void MainWindow::createCacheDisplays() {
  // Crear tablas para las cachés
  int cacheXPositions[] = {100, 412, 724};
  for (int i = 0; i < 3; ++i) {
    QGraphicsRectItem *cacheRect =
        scene->addRect(cacheXPositions[i], 380, 200, 100, QPen(Qt::black),
                       QBrush(Qt::lightGray));
    QGraphicsTextItem *cacheLabel = scene->addText(QString("CACHE %1").arg(i));
    cacheLabel->setPos(cacheXPositions[i] + 70, 350);

    QTableWidget *cacheTable = new QTableWidget(2, 3);
    cacheTable->setHorizontalHeaderLabels(QStringList() << "Tag"
                                                        << "Data"
                                                        << "State");
    cacheTable->verticalHeader()->setVisible(false);
    cacheTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cacheTable->setFixedSize(200, 100);

    // Agregar la tabla a la escena
    QGraphicsProxyWidget *cacheTableProxy = scene->addWidget(cacheTable);
    cacheTableProxy->setPos(cacheXPositions[i], 380);

    cacheTableWidgets.push_back(cacheTable);
  }
}

void MainWindow::createCPUDisplays() {
  // Crear botones para CPUs
  int cpuXPositions[] = {100, 412, 724};
  for (int cpuId = 0; cpuId < 3; ++cpuId) {
    QGraphicsRectItem *cpuRect =
        scene->addRect(cpuXPositions[cpuId], 500, 200, 100, QPen(Qt::black),
                       QBrush(Qt::lightGray));
    QGraphicsTextItem *cpuLabel = scene->addText(QString("CPU %1").arg(cpuId));
    cpuLabel->setPos(cpuXPositions[cpuId] + 70, 470);

    // Crear botones de lectura y escritura para direcciones 0 a 3
    for (int addr = 0; addr < 4; ++addr) {
      QPushButton *readButton = new QPushButton(QString("Read a%1").arg(addr));
      readButton->setFixedSize(90, 25);

      QPushButton *writeButton =
          new QPushButton(QString("Write a%1").arg(addr));
      writeButton->setFixedSize(90, 25);

      // Agregar los botones a la escena
      QGraphicsProxyWidget *readButtonProxy = scene->addWidget(readButton);
      readButtonProxy->setPos(cpuXPositions[cpuId] + 5, 500 + addr * 25);

      QGraphicsProxyWidget *writeButtonProxy = scene->addWidget(writeButton);
      writeButtonProxy->setPos(cpuXPositions[cpuId] + 105, 500 + addr * 25);

      cpuReadButtons.push_back(readButton);
      cpuWriteButtons.push_back(writeButton);
    }
  }
}

void MainWindow::createControlButtons() {
  // Crear botones de control
  resetButton = new QPushButton("Reset");
  helpButton = new QPushButton("Help");

  resetButton->setFixedSize(100, 30);
  helpButton->setFixedSize(100, 30);

  // Agregar los botones a la escena
  QGraphicsProxyWidget *resetButtonProxy = scene->addWidget(resetButton);
  resetButtonProxy->setPos(900, 10);

  QGraphicsProxyWidget *helpButtonProxy = scene->addWidget(helpButton);
  helpButtonProxy->setPos(900, 50);

  // Contador de ciclos de bus
  busCycleLabel = new QLabel("Bus Cycles: 0");
  busCycleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

  // Agregar la etiqueta a la escena
  QGraphicsProxyWidget *busCycleLabelProxy = scene->addWidget(busCycleLabel);
  busCycleLabelProxy->setPos(850, 100);
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
  for (int cpuId = 0; cpuId < 3; ++cpuId) {
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
