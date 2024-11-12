// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "bus.h"
#include "cache.h"
#include "shared_memory.h"

const int NUM_CPUS =
    4;  // Puedes cambiar este valor para ajustar el número de CPUs

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_resetButton_clicked();
  void on_helpButton_clicked();
  void updateDisplays();

  // Slots para botones de CPUs
  void on_cpuReadButton_clicked(int cpuId, int address);
  void on_cpuWriteButton_clicked(int cpuId, int address);

 private:
  // Widgets de la interfaz
  QLabel *titleLabel;
  QLabel *instructionLabel;
  QLabel *busCycleLabel;

  QWidget *centralWidget;
  QVBoxLayout *mainLayout;

  QTableWidget *memoryTableWidget;
  std::vector<QTableWidget *> cacheTableWidgets;

  std::vector<QPushButton *> cpuReadButtons;
  std::vector<QPushButton *> cpuWriteButtons;

  QPushButton *resetButton;
  QPushButton *helpButton;

  QTimer *timer;

  // Objetos de simulación
  Bus systemBus;
  std::vector<Cache *> caches;
  SharedMemory sharedMemory;

  int busCycles;

  // Funciones auxiliares
  void setupUI();
  void createMemoryDisplay();
  void createCacheDisplays();
  void createCPUDisplays();
  void createControlButtons();

  void updateMemoryDisplay();
  void updateCacheDisplays();
  void updateBusCycleCount();
  void resetSimulation();
};

#endif  // MAINWINDOW_H
