#ifndef OCCVIEW_H
#define OCCVIEW_H

#include <QWidget>
#include <QMouseEvent>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>

class OccView : public QWidget
{
private:
    bool isResizePending = true;

    Q_OBJECT
public:
    Handle(AIS_InteractiveContext) context;
    Handle(V3d_View) view;
    Handle(V3d_Viewer) viewer;
    explicit OccView(QWidget *parent = nullptr);

protected:

    QPaintEngine* paintEngine() const {
        return 0;
    }

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

    virtual void mouseMoveEvent(QMouseEvent *event) {
        view->Rotation(event->x(), event->y());
    }



};

#endif // OCCVIEW_H
