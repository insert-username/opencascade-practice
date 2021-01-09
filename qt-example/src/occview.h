#ifndef OCCVIEW_H
#define OCCVIEW_H

#include <QWidget>
#include <QMouseEvent>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>

#include "mouse-input-monitor.h"
#include "camera-controller.h"
#include "select-controller.h"
#include <OpenGl_GraphicDriver.hxx>
#include <QGLWidget>
#include <Aspect_Window.hxx>
#include <Xw_Window.hxx>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>

#include <Aspect_Handle.hxx>
#include <Aspect_Grid.hxx>
#include <AIS_InteractiveContext.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

class OccView : public QGLWidget
{
private:
    bool isResizePending = true;
    MouseInputMonitor mouseInputMonitor;
    CameraController cameraController{};
    SelectController selectController{};

    Q_OBJECT
public:
    Handle(AIS_InteractiveContext) context;
    Handle(V3d_View) view;
    Handle(V3d_Viewer) viewer;
    explicit OccView(QWidget *parent) : QGLWidget(parent) {
        Handle(Aspect_DisplayConnection) displayConnection =
                new Aspect_DisplayConnection();

        Handle(Aspect_Window) window =
                new Xw_Window(displayConnection, (Window)winId());

        Handle(Graphic3d_GraphicDriver) graphicDriver =
                new OpenGl_GraphicDriver(displayConnection);

        viewer = new V3d_Viewer(graphicDriver);

        view = viewer->CreateView();

        view->SetWindow(window);
        if (!window->IsMapped()) {
            window->Map();
        }

        viewer->SetDefaultLights();
        viewer->SetLightOn();

        view->SetBgGradientColors(
                Quantity_NOC_BLACK,
                Quantity_NOC_RED1,
                Aspect_GradientFillMethod::Aspect_GFM_CORNER1,
                false);
        view->MustBeResized();
        view->TriedronDisplay(
                Aspect_TOTP_LEFT_LOWER,
                Quantity_NOC_WHITE,
                0.08,
                V3d_ZBUFFER);
        view->ZBufferTriedronSetup();
        view->Camera()->SetProjectionType(Graphic3d_Camera::Projection_Perspective);

        context = new AIS_InteractiveContext(viewer);
        context->SetDisplayMode(AIS_Shaded, Standard_True);
        context->SetAutoActivateSelection(false);
        context->MainSelector()->AllowOverlapDetection(true);

        auto highlightStyle = context->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected);
        highlightStyle->SetFaceBoundaryDraw(true);
        highlightStyle->SetColor(Quantity_NOC_RED1);
        highlightStyle->SetMethod(Aspect_TOHM_COLOR);

        Graphic3d_RenderingParams& renderParams = view->ChangeRenderingParams();
        renderParams.Method = Graphic3d_RM_RAYTRACING;
        renderParams.RaytracingDepth = 3;
        renderParams.IsShadowEnabled = true;
        renderParams.IsReflectionEnabled = true;
        renderParams.IsAntialiasingEnabled = true;
        renderParams.IsTransparentShadowEnabled = true,

        viewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
        view->SetScale(10);
        view->Update();

        setMouseTracking(true);
    }

protected:

    void paintEvent(QPaintEvent *event) override {
        if (isResizePending) {
            view->MustBeResized();
            isResizePending = false;
        }

        view->Redraw();
    }

    void resizeEvent(QResizeEvent *event) override {
        isResizePending = true;
    }

protected:

    bool event(QEvent *event) override {
        bool isEventProcessed = mouseInputMonitor.accept(event);

        // fallback event handling.
        if (isEventProcessed) {
            cameraController.apply(mouseInputMonitor.poll(), view);
            selectController.apply(mouseInputMonitor.poll(), view, context);
            return true;
        } else {
            return QGLWidget::event(event);
        }
    }

};

#endif // OCCVIEW_H
