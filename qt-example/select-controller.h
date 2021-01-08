//
// Created by luke on 2021-01-04.
//

#ifndef QT_EXAMPLE_SELECT_CONTROLLER_H
#define QT_EXAMPLE_SELECT_CONTROLLER_H

#include "mouse-input-monitor.h"
#include <V3d_Viewer.hxx>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_SelectionManager.hxx>

class SelectController {

public:
    void apply(MouseCommand mouseCommand,
               Handle(V3d_View) view,
               Handle(AIS_InteractiveContext) context) {

        if (mouseCommand.mouseCommandType == CLICK) {
            context->MoveTo(mouseCommand.x, mouseCommand.y, view, false);
            context->MainSelector()->SortResult();
            context->InitDetected();
            if (context->MoreDetected()) {
                context->AddSelect(context->DetectedInteractive());
                view->Update();
            }
        }
    }
};

#endif //QT_EXAMPLE_SELECT_CONTROLLER_H
