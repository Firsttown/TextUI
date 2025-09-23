#ifndef CONTAINER_H
#define CONTAINER_H

#include "Widget.h"
#include <vector>
#include <memory>

class Container : public Widget {
protected:
    std::vector<std::unique_ptr<Widget>> children;
    Widget* focusedChild;

public:
    Container(int x, int y, int width, int height) 
        : Widget(x, y, width, height), focusedChild(nullptr) {}
    
    template<typename T, typename... Args>
    T* addChild(Args&&... args) {
        auto widget = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = widget.get();
        children.push_back(std::move(widget));
        return ptr;
    }
    
    void removeChild(Widget* widget) {
        auto it = std::find_if(children.begin(), children.end(),
            [widget](const std::unique_ptr<Widget>& w) { return w.get() == widget; });
        if (it != children.end()) {
            if (focusedChild == widget) {
                focusedChild = nullptr;
            }
            children.erase(it);
        }
    }
    
    Widget* getFocusedChild() const { return focusedChild; }
    void setFocusedChild(Widget* widget) { focusedChild = widget; }
    
    const std::vector<std::unique_ptr<Widget>>& getChildren() const { return children; }
    
    void render() override {
        if (!visible) return;
        
        // Рисуем дочерние виджеты
        for (auto& child : children) {
            if (child->isVisible()) {
                child->render();
            }
        }
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible) return;
        
        // Рисуем дочерние виджеты
        for (auto& child : children) {
            if (child->isVisible()) {
                child->renderToBuffer(buffer);
            }
        }
        
        markClean();
    }
    
    bool handleInput(char key) override {
        if (!visible) return false;
        
        // Передаем ввод фокусированному дочернему виджету
        if (focusedChild && focusedChild->isVisible()) {
            if (focusedChild->handleInput(key)) {
                return true;
            }
        }
        
        // Если фокусированный виджет не обработал, пробуем остальные
        for (auto& child : children) {
            if (child.get() != focusedChild && child->isVisible()) {
                if (child->handleInput(key)) {
                    // Переключаем фокус
                    focusedChild = child.get();
                    return true;
                }
            }
        }
        
        return false;
    }
    
    bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) override {
        if (!visible) return false;
        
        // Проверяем дочерние виджеты в обратном порядке (верхние первыми)
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if ((*it)->isVisible() && (*it)->isPointInside(mouseX, mouseY)) {
                if ((*it)->handleMouse(mouseX, mouseY, button, isPress)) {
                    // Устанавливаем фокус на виджет
                    focusedChild = it->get();
                    return true;
                }
            }
        }
        
        // Клик вне дочерних виджетов - снимаем фокус
        if (button == MouseButton::LEFT && isPress) {
            focusedChild = nullptr;
        }
        
        return isPointInside(mouseX, mouseY);
    }
    
    bool handleMouseMove(int mouseX, int mouseY) override {
        if (!visible) return false;
        
        bool handled = false;
        
        // Передаем движение мыши всем дочерним виджетам
        for (auto& child : children) {
            if (child->isVisible()) {
                if (child->handleMouseMove(mouseX, mouseY)) {
                    handled = true;
                }
            }
        }
        
        return handled;
    }
    
    Widget* getWidgetAt(int pointX, int pointY) {
        // Проверяем дочерние виджеты в обратном порядке
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if ((*it)->isVisible() && (*it)->isPointInside(pointX, pointY)) {
                // Если это контейнер, рекурсивно проверяем его дочерние элементы
                if (auto* container = dynamic_cast<Container*>(it->get())) {
                    Widget* subWidget = container->getWidgetAt(pointX, pointY);
                    if (subWidget) return subWidget;
                }
                return it->get();
            }
        }
        return nullptr;
    }
};

#endif