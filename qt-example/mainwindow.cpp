#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->occView = new OccView(this);
    this->setCentralWidget(this->occView);

    connect(ui->actionAddBox, SIGNAL(triggered()), this, SLOT(addBox()));
}

void MainWindow::addBox() {
    std::cout << "Adding box" << std::endl;
    TopoDS_Shape shape = BRepPrimAPI_MakeBox(3.0, 4.0, 5.0).Shape();
    Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
    aisShape->SetColor(Quantity_NOC_AZURE);


    std::cout << "Adding box to context" << std::endl;
    this->occView->context->Display(aisShape, Standard_True);
    this->occView->view->FitAll();
    this->occView->view->ZFitAll();
    this->occView->view->Redraw();
}

MainWindow::~MainWindow()
{
    delete ui;
}
