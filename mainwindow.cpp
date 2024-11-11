#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  btnStep = new QPushButton("Step", this);
  btnStep->setGeometry(10, 10, 80, 30);

  labelStatus = new QLabel("Status: Ready", this);
  labelStatus->setGeometry(10, 50, 200, 30);

  // Conecta la señal del botón con la lógica de stepping
  connect(btnStep, &QPushButton::clicked, [this]() {
    // Aquí llama a la función que maneja el paso en el sistema
    labelStatus->setText("Stepping...");
  });
}
