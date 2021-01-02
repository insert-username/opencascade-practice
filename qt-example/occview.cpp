#include "occview.h"

#include <OpenGl_GraphicDriver.hxx>

#include <Xw_Window.hxx>
#include <V3d_View.hxx>

#include <Aspect_Handle.hxx>
#include <AIS_InteractiveContext.hxx>


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

    Handle(Graphic3d_GraphicDriver) graphicDriver =
            GetGraphicDriver(displayConnection);

    Handle(Xw_Window) window =
            new Xw_Window(displayConnection, (Window)winId());

    viewer = new V3d_Viewer(graphicDriver);

    view = viewer->CreateView();

    view->SetWindow(window);
    if (!window->IsMapped()) {
        window->Map();
    }

    Handle(AIS_InteractiveContext) interactiveContext =
            new AIS_InteractiveContext(viewer);

    viewer->SetDefaultLights();
    viewer->SetLightOn();

    view->SetBackgroundColor(Quantity_NOC_BLACK);
    view->MustBeResized();
    view->TriedronDisplay(
                Aspect_TOTP_LEFT_LOWER,
                Quantity_NOC_GOLD,
                0.08,
                V3d_ZBUFFER);

    interactiveContext->SetDisplayMode(AIS_Shaded, Standard_True);
}

QPaintEngine* OccView::paintEngine() const {
    return 0;
}

void OccView::paintEvent(QPaintEvent *event) {
    view->Redraw();
}

void OccView::resizeEvent(QResizeEvent *event) {
    if (!view.IsNull()) {
        view->MustBeResized();
    }
}
