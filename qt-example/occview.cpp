#include "occview.h"

#include <OpenGl_GraphicDriver.hxx>

#include <Xw_Window.hxx>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>

#include <Aspect_Handle.hxx>
#include <AIS_InteractiveContext.hxx>
#include "MakeBottle.h"
#include <BRepPrimAPI_MakeBox.hxx>

static Handle(Graphic3d_GraphicDriver) GetGraphicDriver(Handle(Aspect_DisplayConnection) &displayConnection) {
    static Handle(Graphic3d_GraphicDriver) driver;
    if (driver.IsNull()) {
        driver = new OpenGl_GraphicDriver(displayConnection);
    }

    return driver;
}

OccView::OccView(QWidget *parent) : QWidget(parent)
{
    Handle(Aspect_DisplayConnection) displayConnection =
            new Aspect_DisplayConnection();

    Handle(Xw_Window) window =
            new Xw_Window(displayConnection, (Window)winId());

    Handle(Graphic3d_GraphicDriver) graphicDriver =
            GetGraphicDriver(displayConnection);

    viewer = new V3d_Viewer(graphicDriver);

    view = viewer->CreateView();

    view->SetWindow(window);
    if (!window->IsMapped()) {
        window->Map();
    }

    viewer->SetDefaultLights();
    viewer->SetLightOn();

    view->SetBackgroundColor(Quantity_NOC_BLACK);
    view->MustBeResized();
    view->TriedronDisplay(
                Aspect_TOTP_LEFT_LOWER,
                Quantity_NOC_WHITE,
                0.08,
                V3d_ZBUFFER);
    view->ZBufferTriedronSetup();

    context = new AIS_InteractiveContext(viewer);
    context->SetDisplayMode(AIS_Shaded, Standard_True);

    view->FitAll();
    view->ZFitAll();
    view->Redraw();

    setMouseTracking(true);
}
