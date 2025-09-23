#include "Mouse.h"
#include "../platform/Platform.h"

MousePosition Mouse::currentPosition = {0, 0, false, false, false};
std::function<void(int, int, MouseButton)> Mouse::onClickHandler = nullptr;
std::function<void(int, int, MouseButton)> Mouse::onReleaseHandler = nullptr;
std::function<void(int, int)> Mouse::onMoveHandler = nullptr;
std::function<void(int, int)> Mouse::onDragHandler = nullptr;
bool Mouse::mouseEnabled = false;
bool Mouse::dragging = false;
int Mouse::dragStartX = 0;
int Mouse::dragStartY = 0;

bool Mouse::enableMouse() {
    mouseEnabled = Platform::enableMouseInput();
    return mouseEnabled;
}

void Mouse::disableMouse() {
    Platform::disableMouseInput();
    mouseEnabled = false;
    reset();
}

bool Mouse::update() {
    if (!mouseEnabled) return false;
    
    int x, y;
    MouseButton button;
    bool clicked, released, moved;
    
    // Получаем события мыши от платформы
    clicked = Platform::getMouseClick(x, y, button);
    released = Platform::getMouseRelease(x, y, button);
    moved = Platform::getMouseMove(x, y);
    
    bool eventHandled = false;
    
    if (clicked) {
        currentPosition.x = x;
        currentPosition.y = y;
        
        switch (button) {
            case MouseButton::LEFT:
                currentPosition.leftPressed = true;
                dragStartX = x;
                dragStartY = y;
                break;
            case MouseButton::RIGHT:
                currentPosition.rightPressed = true;
                break;
            case MouseButton::MIDDLE:
                currentPosition.middlePressed = true;
                break;
        }
        
        if (onClickHandler) {
            onClickHandler(x, y, button);
        }
        eventHandled = true;
    }
    
    if (released) {
        currentPosition.x = x;
        currentPosition.y = y;
        
        switch (button) {
            case MouseButton::LEFT:
                currentPosition.leftPressed = false;
                dragging = false;
                break;
            case MouseButton::RIGHT:
                currentPosition.rightPressed = false;
                break;
            case MouseButton::MIDDLE:
                currentPosition.middlePressed = false;
                break;
        }
        
        if (onReleaseHandler) {
            onReleaseHandler(x, y, button);
        }
        eventHandled = true;
    }
    
    if (moved) {
        currentPosition.x = x;
        currentPosition.y = y;
        
        if (onMoveHandler) {
            onMoveHandler(x, y);
        }
        
        // Проверяем перетаскивание
        if (currentPosition.leftPressed) {
            if (!dragging && 
                (abs(x - dragStartX) > 1 || abs(y - dragStartY) > 1)) {
                dragging = true;
            }
            
            if (dragging && onDragHandler) {
                onDragHandler(x, y);
            }
        }
        eventHandled = true;
    }
    
    return eventHandled;
}

void Mouse::reset() {
    currentPosition = {0, 0, false, false, false};
    dragging = false;
    dragStartX = 0;
    dragStartY = 0;
}