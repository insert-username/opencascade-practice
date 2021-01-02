#ifndef OCCVIEW_H
#define OCCVIEW_H

#include <QWidget>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>

class OccView : public QWidget
{
    Q_OBJECT
public:
    explicit OccView(QWidget *parent = nullptr);

protected:
    virtual QPaintEngine* paintEngine() const;

    virtual void paintEvent(QPaintEvent *event);

    virtual void resizeEvent(QResizeEvent *event);
signals:


private:
    Handle(V3d_Viewer) viewer;
    Handle(V3d_View) view;
    Handle(AIS_InteractiveContext) context;
};

#endif // OCCVIEW_H
