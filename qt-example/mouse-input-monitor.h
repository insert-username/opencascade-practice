#ifndef MOUSEINPUTMONITOR_H
#define MOUSEINPUTMONITOR_H

#include <QMouseEvent>

enum MouseCommandType {
    NONE,
    DRAG,
    DRAG_START,
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

    static bool isAnyButtonDown(QMouseEvent* mouseEvent) {
        auto buttons = mouseEvent->buttons();

        return (buttons & Qt::LeftButton) ||
               (buttons & Qt::MiddleButton) ||
               (buttons & Qt::RightButton);
    }

    // tracks a single contiguous drag operation.
    class DragStateMonitor {
        int xDragStart{};
        int yDragStart{};

        int xDragCurrent{};
        int yDragCurrent{};

        bool isMouseDown = false;
        bool isDragInProgress = false;

    public:
        // need to process at least two events to
        // qualify as a drag.
        void accept(QMouseEvent* mouseEvent) {
            if (isAnyButtonDown(mouseEvent)) {
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
                mouseCommand.mouseCommandType = DRAG;
            } else if (isMouseDown) {
                mouseCommand.mouseCommandType = DRAG_START;
            } else {
                mouseCommand.mouseCommandType = NONE;
            }

            mouseCommand.x = xDragCurrent;
            mouseCommand.y = yDragCurrent;
            mouseCommand.dx = xDragCurrent - xDragStart;
            mouseCommand.dy = yDragCurrent - yDragStart;

            return true;
        }
    };

    //
    class ClickStateMonitor {
    private:
        // indicates if the mouse was down
        // the last time the event was triggered.
        bool wasLastEventMouseDown = false;
        QPoint initialMouseDownPoint;
        bool hasMouseDragged = true;

        bool hasClicked = false;
        QPoint clickPoint;

    public:
        void accept(QMouseEvent* mouseEvent) {
            if (isAnyButtonDown(mouseEvent)) {
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
                return true;
            } else {
                return false;
            }
        }
    };

    DragStateMonitor dragStateMonitor;
    ClickStateMonitor clickStateMonitor;

    MouseCommand currentMouseCommand{};

public:
    // accept the latest mouse event.
    // note that the next polled command
    // may be overwritten by this.
    void accept(QMouseEvent* mouseEvent) {
        dragStateMonitor.accept(mouseEvent);
        clickStateMonitor.accept(mouseEvent);

        if (!clickStateMonitor.poll(currentMouseCommand)) {
            dragStateMonitor.poll(currentMouseCommand);
        }
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
