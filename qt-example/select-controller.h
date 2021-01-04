//
// Created by luke on 2021-01-04.
//

#ifndef QT_EXAMPLE_SELECT_CONTROLLER_H
#define QT_EXAMPLE_SELECT_CONTROLLER_H

#include "mouse-input-monitor.h"
#include <V3d_Viewer.hxx>
#include <TopoDS_Shape.hxx>

class SelectController {

public:
    void apply(MouseCommand mouseCommand,
               Handle(V3d_View) view,
               Handle(AIS_InteractiveContext) context) {

        if (mouseCommand.mouseCommandType == CLICK) {
            context->Select(
                    mouseCommand.x,
                    mouseCommand.y,
                    mouseCommand.x + 1,
                    mouseCommand.y + 1,
                    view,
                    true
                    );
        }
    }
};

#endif //QT_EXAMPLE_SELECT_CONTROLLER_H
