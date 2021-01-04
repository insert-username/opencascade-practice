#ifndef OCCVIEW_H
#define OCCVIEW_H

#include <QWidget>
#include <QMouseEvent>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>

#include "mouse-input-monitor.h"
#include "camera-controller.h"
#include <OpenGl_GraphicDriver.hxx>
#include <QGLWidget>

class OccView : public QGLWidget
{
private:
    bool isResizePending = true;
    MouseInputMonitor mouseInputMonitor;
    CameraController cameraController;

    Q_OBJECT
public:
    Handle(AIS_InteractiveContext) context;
    Handle(V3d_View) view;
    Handle(V3d_Viewer) viewer;
    explicit OccView(QWidget *parent = nullptr);

protected:

    void paintEvent(QPaintEvent *event) {
        if (isResizePending) {
            view->MustBeResized();
            isResizePending = false;
        }

        view->Redraw();
    }

    void resizeEvent(QResizeEvent *event) {
        isResizePending = true;
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        mouseInputMonitor.accept(event);
        cameraController.apply(mouseInputMonitor.poll(), view);
    }

    void mousePressEvent(QMouseEvent *event) override {
        mouseInputMonitor.accept(event);
        cameraController.apply(mouseInputMonitor.poll(), view);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        mouseInputMonitor.accept(event);
        cameraController.apply(mouseInputMonitor.poll(), view);
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override {
        mouseInputMonitor.accept(event);
        cameraController.apply(mouseInputMonitor.poll(), view);
    }

};

#endif // OCCVIEW_H
