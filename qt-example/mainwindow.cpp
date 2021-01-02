#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <OpenGl_GraphicDriver.hxx>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(new OccView(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}
