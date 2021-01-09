#ifndef MOUSEINPUTMONITOR_H
#define MOUSEINPUTMONITOR_H

#include <QMouseEvent>

enum MouseCommandType {
    NONE,
    ORBIT,
    ORBIT_START,
    PAN,
    PAN_START,
    SCROLL,
    CLICK
};

struct MouseCommand {

    // absolute coordinates associated with the commmand.
    int x;
    int y;

    // any coordinate delta associated with the command.
    int dx;
    int dy;

    MouseCommandType mouseCommandType;
};

class MouseInputMonitor {
private:

    static bool isButtonDown(QMouseEvent* mouseEvent, u_int8_t btnMap) {
        auto buttons = mouseEvent->buttons();

        return buttons & btnMap;
    }

    // tracks a single contiguous drag operation.
    class DragStateMonitor {

        uint8_t btnMap;
        MouseCommandType dragStartCommand;
        MouseCommandType dragCommand;

        int xDragStart{};
        int yDragStart{};

        int xDragCurrent{};
        int yDragCurrent{};

        bool isMouseDown = false;
        bool isDragInProgress = false;

    public:
        DragStateMonitor(uint8_t btnMap,
                         MouseCommandType dragStartCommand,
                         MouseCommandType dragCommand) :
                            btnMap(btnMap),
                            dragStartCommand(dragStartCommand),
                            dragCommand(dragCommand) {

        }

        // need to process at least two events to
        // qualify as a drag.
        void accept(QEvent* event) {
            if (event->type() != QEvent::MouseMove) {

                // nothing to do.
                return;
            }

            auto mouseEvent = static_cast<QMouseEvent*>(event);

            if (isButtonDown(mouseEvent, btnMap)) {
                if (!isMouseDown) {
                    xDragStart = mouseEvent->x();
                    yDragStart = mouseEvent->y();
                    isMouseDown = true;
                } else {
                    isDragInProgress = true;
                    xDragCurrent = mouseEvent->x();
                    yDragCurrent = mouseEvent->y();
                }
            } else {
                isMouseDown = false;
                isDragInProgress = false;
            }
        }

        bool poll(MouseCommand &mouseCommand) const {
            // if not dragging, do nothing.
            if (isDragInProgress) {
                mouseCommand.mouseCommandType = dragCommand;
            } else if (isMouseDown) {
                mouseCommand.mouseCommandType = dragStartCommand;
            } else {
                mouseCommand.mouseCommandType = NONE;
            }

            mouseCommand.x = xDragCurrent;
            mouseCommand.y = yDragCurrent;
            mouseCommand.dx = xDragCurrent - xDragStart;
            mouseCommand.dy = yDragCurrent - yDragStart;

            return mouseCommand.mouseCommandType != NONE;
        }
    };

    //
    class ClickStateMonitor {
    private:
        uint8_t btnMap;

        // indicates if the mouse was down
        // the last time the event was triggered.
        bool wasLastEventMouseDown = false;
        QPoint initialMouseDownPoint;
        bool hasMouseDragged = true;

        bool hasClicked = false;
        QPoint clickPoint;

    public:
        ClickStateMonitor(uint8_t btnMap) : btnMap(btnMap) {

        }

        void accept(QEvent* event) {

            if (event->type() != QEvent::MouseButtonPress &&
                event->type() != QEvent::MouseButtonRelease &&
                event->type() != QEvent::MouseMove) {

                // nothing to do.
                return;
            }

            auto mouseEvent = static_cast<QMouseEvent*>(event);

            if (isButtonDown(mouseEvent, btnMap)) {
                if (wasLastEventMouseDown) {
                    hasMouseDragged |= mouseEvent->pos() != initialMouseDownPoint;
                } else {
                    initialMouseDownPoint = mouseEvent->pos();
                    hasMouseDragged = false;
                }

                wasLastEventMouseDown = true;
            } else {

                if (wasLastEventMouseDown && !hasMouseDragged) {
                    hasClicked = true;
                    clickPoint = mouseEvent->pos();
                } else {
                    hasClicked = false;
                }

                wasLastEventMouseDown = false;
            }
        }

        bool poll(MouseCommand &mouseCommand) {
            if (hasClicked) {
                mouseCommand.x = clickPoint.x();
                mouseCommand.y = clickPoint.y();
                mouseCommand.mouseCommandType = CLICK;
                hasClicked = false;
                wasLastEventMouseDown = false;
                return true;
            } else {
                return false;
            }
        }
    };

    class ScrollStateMonitor {
    private:
        int scrollAmount = 0;

    public:
        void accept(QEvent* event) {
            if (event->type() == QEvent::Wheel) {
                auto wheelEvent = static_cast<QWheelEvent*>(event);
                scrollAmount = wheelEvent->delta();
            }
        }

        bool poll(MouseCommand &mouseCommand) {
            if (scrollAmount != 0) {
                mouseCommand.mouseCommandType = SCROLL;
                mouseCommand.dy = scrollAmount;
                scrollAmount = 0;
                return true;
            }

            return false;
        }
    };

    DragStateMonitor dragOrbitStateMonitor;
    DragStateMonitor dragPanStateMonitor;
    ClickStateMonitor clickStateMonitor;
    ScrollStateMonitor scrollStateMonitor;

    MouseCommand currentMouseCommand{};

public:
    MouseInputMonitor() :
            dragOrbitStateMonitor(Qt::LeftButton, ORBIT_START, ORBIT),
            dragPanStateMonitor(Qt::MiddleButton, PAN_START, PAN),
            clickStateMonitor(Qt::LeftButton),
            scrollStateMonitor() {

    }

    // accept the latest mouse event.
    // note that the next polled command
    // may be overwritten by this.
    bool accept(QEvent* mouseEvent) {
        dragOrbitStateMonitor.accept(mouseEvent);
        dragPanStateMonitor.accept(mouseEvent);
        clickStateMonitor.accept(mouseEvent);
        scrollStateMonitor.accept(mouseEvent);

        return clickStateMonitor.poll(currentMouseCommand) ||
               dragOrbitStateMonitor.poll(currentMouseCommand) ||
               dragPanStateMonitor.poll(currentMouseCommand) ||
               scrollStateMonitor.poll(currentMouseCommand);
    }

    // returns true if a mouse command is available to
    // be processed.
    MouseCommand poll() {
        return currentMouseCommand;
    }

    static void printMouseCommand(MouseCommand &mouseCommand){
        if (mouseCommand.mouseCommandType == NONE) {
            return;
        }

        std::cout << "Mouse command: " << std::endl;
        std::cout << "    type " << mouseCommand.mouseCommandType << std::endl;
        std::cout << "    dx " << mouseCommand.dx << std::endl;
        std::cout << "    dy " << mouseCommand.dy << std::endl;
        std::cout << "    x " << mouseCommand.x << std::endl;
        std::cout << "    y " << mouseCommand.y << std::endl;
    }
};

#endif // MOUSEINPUTMONITOR_H
