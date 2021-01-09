#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "mouse-input-monitor.h"

#include <cmath>
#include <V3d_View.hxx>

// Defines a utility class for translating mouse input into
// camera movements.

class CameraController {

    constexpr static const double MOUSE_SENSITIVITY = 0.005;

    Standard_Real startX;
    Standard_Real startY;
    Standard_Real startZ;
    double startZAxAngle;
    double startHeightAngle;
    double startR;
public:
    void apply(MouseCommand mouseCommand, Handle(V3d_View) view) {
        if (mouseCommand.mouseCommandType == DRAG_START) {
            view->Eye(startX, startY, startZ);

            // determine angle
            startZAxAngle = std::atan2(startX, startY);

            double xyHypot = std::hypot(startX, startY);

            startHeightAngle = std::atan2(startZ, xyHypot);

            startR = std::hypot(startZ, xyHypot);
        } else if (mouseCommand.mouseCommandType == DRAG) {
            double dragHeightAngle = std::max(std::min(
                    mouseCommand.dy * MOUSE_SENSITIVITY + startHeightAngle,
                    M_PI_2), -M_PI_2);
            double dragZAxAngle = mouseCommand.dx * MOUSE_SENSITIVITY + startZAxAngle;

            const double rxy = startR * std::cos(dragHeightAngle);
            const double rz = startR * std::sin(dragHeightAngle);

            view->SetEye(
                    rxy * std::sin(dragZAxAngle),
                    rxy * std::cos(dragZAxAngle),
                    rz);
        } else if (mouseCommand.mouseCommandType == SCROLL) {
            auto scrollAmount = mouseCommand.dy;

            view->Zoom(0, 0, scrollAmount, 0);
        }

    }
};

#endif // CAMERACONTROLLER_H
