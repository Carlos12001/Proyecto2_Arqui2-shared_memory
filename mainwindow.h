// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "PE.h"
#include "bus.h"
#include "cache.h"
#include "shared_memory.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_readButton_clicked();
  void on_writeButton_clicked();
  void on_resetButton_clicked();
  void on_helpButton_clicked();
  void updateDisplays();

 private:
  // Widgets de la interfaz
  QLabel *titleLabel;
  QLabel *instructionLabel;
  QLabel *busCycleLabel;

  QTableWidget *memoryTableWidget;
  QTableWidget *cache0TableWidget;
  QTableWidget *cache1TableWidget;

  QLineEdit *addressLineEdit;
  QLineEdit *valueLineEdit;

  QPushButton *readButton;
  QPushButton *writeButton;
  QPushButton *resetButton;
  QPushButton *helpButton;

  QTimer *timer;

  // Objetos de simulación
  Bus systemBus;
  Cache cache0;
  Cache cache1;
  SharedMemory sharedMemory;

  int busCycles;

  // Funciones auxiliares
  void setupUI();
  void updateMemoryDisplay();
  void updateCacheDisplay();
  void updateBusCycleCount();
};

#endif  // MAINWINDOW_H
