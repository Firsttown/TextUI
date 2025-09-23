#ifndef MOUSE_H
#define MOUSE_H

#include <functional>

enum class MouseButton {
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2
};

struct MousePosition {
    int x, y;
    bool leftPressed;
    bool rightPressed;
    bool middlePressed;
};

class Mouse {
private:
    static MousePosition currentPosition;
    static std::function<void(int, int, MouseButton)> onClickHandler;
    static std::function<void(int, int, MouseButton)> onReleaseHandler;
    static std::function<void(int, int)> onMoveHandler;
    static std::function<void(int, int)> onDragHandler;
    static bool mouseEnabled;
    static bool dragging;
    static int dragStartX, dragStartY;

public:
    static bool enableMouse();
    static void disableMouse();
    static bool isMouseEnabled() { return mouseEnabled; }
    
    static const MousePosition& getCurrentPosition() { return currentPosition; }
    
    static void setOnClickHandler(const std::function<void(int, int, MouseButton)>& handler) {
        onClickHandler = handler;
    }
    
    static void setOnReleaseHandler(const std::function<void(int, int, MouseButton)>& handler) {
        onReleaseHandler = handler;
    }
    
    static void setOnMoveHandler(const std::function<void(int, int)>& handler) {
        onMoveHandler = handler;
    }
    
    static void setOnDragHandler(const std::function<void(int, int)>& handler) {
        onDragHandler = handler;
    }
    
    static bool update(); // Возвращает true если были события мыши
    static void reset();
};

#endif