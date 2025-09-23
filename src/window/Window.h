#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include <string>
#include <functional>
#include "src/widgets/Container.h"
#include "src/core/Color.h"
#include "src/core/BoxStyle.h"
#include "src/input/Mouse.h"

class Window : public Container {
private:
    std::string title;
    bool hasFocus;
    bool modal;
    Window* parent;
    std::vector<Window*> childWindows;
    bool dragging;
    int dragOffsetX, dragOffsetY;
    std::function<void()> onClose;
    bool isClosing;

public:
    Window(int x, int y, int width, int height, const std::string& title = "");
    ~Window();
    
    void setTitle(const std::string& newTitle) { title = newTitle; }
    const std::string& getTitle() const { return title; }
    
    void setModal(bool value) { modal = value; }
    bool isModal() const { return modal; }
    
    void setParent(Window* parentWindow) { parent = parentWindow; }
    Window* getParent() const { return parent; }
    
    void setOnClose(const std::function<void()>& callback) { onClose = callback; }
    
    Window* createChildWindow(int x, int y, int width, int height, const std::string& title = "");
    
    void setFocus(bool focus) { hasFocus = focus; }
    bool hasFocus() const { return hasFocus; }
    
    void render() override;
    bool handleInput(char key) override;
    bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) override;
    bool handleMouseMove(int mouseX, int mouseY) override;
    
    bool isPointInside(int pointX, int pointY) const override;
    Widget* getWidgetAt(int pointX, int pointY) override;
    Window* getWindowAt(int pointX, int pointY);
    
    void bringToFront();
    void close();
    
    // Методы для перетаскивания
    void startDragging(int mouseX, int mouseY);
    void updateDragPosition(int mouseX, int mouseY);
    void stopDragging();
    
    bool isDragging() const { return dragging; }
    
    // Методы для работы с дочерними окнами
    const std::vector<Window*>& getChildWindows() const { return childWindows; }
    void removeChildWindow(Window* child);
    
    bool isClosing() const { return isClosing; }
};

#endif