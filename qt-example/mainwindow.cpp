#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto* viewportWidget = ui->centralwidget->findChild<QWidget*>("viewportWidget");

    this->occView = new OccView(viewportWidget);

    auto* layout = new QGridLayout(viewportWidget);
    layout->addWidget(this->occView);

    connect(ui->actionAddBox, SIGNAL(triggered()), this, SLOT(addBox()));
}

void MainWindow::addBox() {
    std::cout << "Adding box" << std::endl;
    double xSize = 3.0 * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    double ySize = 3.0 * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    double zSize = 3.0 * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

    TopoDS_Shape shape = BRepPrimAPI_MakeBox(xSize, ySize, zSize).Shape();
    Handle(AIS_Shape) aisShape = new AIS_Shape(shape);

    Graphic3d_MaterialAspect matAspect;
    matAspect.SetDiffuseColor(Quantity_NOC_BEET);
    matAspect.SetAmbientColor(Quantity_NOC_BEET);
    matAspect.SetSpecularColor(Quantity_NOC_GREEN1);

    aisShape->SetMaterial(matAspect);

    this->occView->context->Display(aisShape, Standard_True);

    this->occView->context->Load(aisShape, -1);
    this->occView->context->Activate(aisShape, TopAbs_FACE);

    this->occView->view->FitAll();
    this->occView->view->ZFitAll();
    this->occView->view->Redraw();
}

MainWindow::~MainWindow()
{
    delete ui;
}
