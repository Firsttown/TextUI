#include "Window.h"
#include "src/core/Renderer.h"
#include "src/widgets/Label.h"
#include "src/widgets/Button.h"
#include <algorithm>
#include <iostream>
#include <cmath>

Window::Window(int x, int y, int width, int height, const std::string& title)
    : Container(x, y, width, height), title(title), hasFocus(false), modal(false), 
      parent(nullptr), dragging(false), dragOffsetX(0), dragOffsetY(0), 
      onClose(nullptr), isClosing(false) {
    boxStyle = BoxStyle::doubleLine();
    colorStyle = ColorStyle(Color::BRIGHT_WHITE, BackgroundColor::DEFAULT).setBold();
}

Window::~Window() {
    // Уведомляем родителя об удалении
    if (parent) {
        parent->removeChildWindow(this);
    }
    
    // Удаляем дочерние окна
    for (auto* child : childWindows) {
        child->setParent(nullptr);
    }
}

Window* Window::createChildWindow(int x, int y, int width, int height, const std::string& title) {
    auto* childWindow = new Window(x, y, width, height, title);
    childWindow->setParent(this);
    childWindows.push_back(childWindow);
    return childWindow;
}

void Window::render() {
    if (!visible || isClosing) return;
    
    ColorStyle windowStyle = colorStyle;
    if (hasFocus) {
        windowStyle = ColorStyle(Color::BRIGHT_WHITE, BackgroundColor::BLACK).setBold();
    }
    
    // Рисуем рамку окна
    Renderer::drawRectangle(x, y, width, height, boxStyle, windowStyle);
    
    // Рисуем заголовок
    if (!title.empty()) {
        int titleX = x + 2;
        if (titleX + static_cast<int>(title.length()) < x + width - 2) {
            std::string titleText = " " + title + " ";
            Renderer::drawText(titleX, y, titleText, 
                             ColorStyle(Color::BLACK, BackgroundColor::WHITE).setBold());
        }
    }
    
    // Рисуем дочерние виджеты (через Container)
    Container::render();
    
    // Рисуем дочерние окна
    for (auto* childWindow : childWindows) {
        childWindow->render();
    }
}

bool Window::handleInput(char key) {
    if (!visible || isClosing) return false;
    
    // Обработка специальных клавиш окна
    if (key == 27) { // Escape - закрыть окно
        close();
        return true;
    }
    
    // Передаем ввод дочерним виджетам (через Container)
    return Container::handleInput(key);
}

bool Window::handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) {
    if (!visible || isClosing) return false;
    
    // Проверяем дочерние окна (в обратном порядке - верхние первыми)
    for (auto it = childWindows.rbegin(); it != childWindows.rend(); ++it) {
        if ((*it)->handleMouse(mouseX, mouseY, button, isPress)) {
            return true;
        }
    }
    
    // Обработка событий мыши для самого окна
    if (button == MouseButton::LEFT) {
        if (isPress) {
            // Клик по заголовку окна - начинаем перетаскивание
            if (mouseY == y && mouseX > x + 1 && mouseX < x + width - 1) {
                startDragging(mouseX, mouseY);
                bringToFront();
                return true;
            }
            
            // Клик по остальному окну - фокус и bring to front
            bringToFront();
        } else {
            // Отпускание кнопки
            if (dragging) {
                stopDragging();
                return true;
            }
        }
    }
    
    // Передаем событие дочерним виджетам
    return Container::handleMouse(mouseX, mouseY, button, isPress);
}

bool Window::handleMouseMove(int mouseX, int mouseY) {
    if (!visible || isClosing) return false;
    
    // Передаем движение дочерним окнам
    bool handled = false;
    for (auto it = childWindows.rbegin(); it != childWindows.rend(); ++it) {
        if ((*it)->handleMouseMove(mouseX, mouseY)) {
            handled = true;
        }
    }
    
    // Обработка перетаскивания окна
    if (dragging) {
        updateDragPosition(mouseX, mouseY);
        return true;
    }
    
    // Передаем движение дочерним виджетам
    if (Container::handleMouseMove(mouseX, mouseY)) {
        handled = true;
    }
    
    return handled;
}

bool Window::isPointInside(int pointX, int pointY) const {
    return pointX >= x && pointX < x + width &&
           pointY >= y && pointY < y + height;
}

Widget* Window::getWidgetAt(int pointX, int pointY) {
    // Проверяем дочерние окна первыми
    for (auto it = childWindows.rbegin(); it != childWindows.rend(); ++it) {
        if ((*it)->isPointInside(pointX, pointY)) {
            return (*it)->getWidgetAt(pointX, pointY);
        }
    }
    
    // Проверяем дочерние виджеты
    return Container::getWidgetAt(pointX, pointY);
}

Window* Window::getWindowAt(int pointX, int pointY) {
    // Проверяем дочерние окна
    for (auto it = childWindows.rbegin(); it != childWindows.rend(); ++it) {
        if ((*it)->isPointInside(pointX, pointY)) {
            return (*it)->getWindowAt(pointX, pointY);
        }
    }
    
    // Если точка внутри этого окна
    if (isPointInside(pointX, pointY)) {
        return this;
    }
    
    return nullptr;
}

void Window::bringToFront() {
    if (parent) {
        // Удаляем себя из списка дочерних окон родителя
        auto& siblings = parent->childWindows;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        // Добавляем себя в конец (на передний план)
        siblings.push_back(this);
    }
    
    // Устанавливаем фокус
    if (parent) {
        parent->setFocus(false);
    }
    setFocus(true);
}

void Window::close() {
    isClosing = true;
    visible = false;
    
    if (onClose) {
        onClose();
    }
    
    if (parent) {
        parent->removeChildWindow(this);
    }
}

void Window::startDragging(int mouseX, int mouseY) {
    dragging = true;
    dragOffsetX = mouseX - x;
    dragOffsetY = mouseY - y;
}

void Window::updateDragPosition(int mouseX, int mouseY) {
    if (!dragging) return;
    
    int newX = mouseX - dragOffsetX;
    int newY = mouseY - dragOffsetY;
    
    // Ограничиваем перемещение границами родительского окна или экрана
    if (parent) {
        // Ограничения внутри родительского окна
        newX = std::max(parent->x + 1, std::min(newX, parent->x + parent->width - width - 1));
        newY = std::max(parent->y + 1, std::min(newY, parent->y + parent->height - height - 1));
    } else {
        // Ограничения экраном (примерные значения)
        newX = std::max(0, std::min(newX, 120 - width));
        newY = std::max(0, std::min(newY, 40 - height));
    }
    
    x = newX;
    y = newY;
}

void Window::stopDragging() {
    dragging = false;
    dragOffsetX = 0;
    dragOffsetY = 0;
}

void Window::removeChildWindow(Window* child) {
    auto it = std::find(childWindows.begin(), childWindows.end(), child);
    if (it != childWindows.end()) {
        childWindows.erase(it);
    }
}