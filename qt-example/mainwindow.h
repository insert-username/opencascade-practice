#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "occview.h"
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void addBox();

private:
    Ui::MainWindow *ui;
    OccView* occView;
};
#endif // MAINWINDOW_H
