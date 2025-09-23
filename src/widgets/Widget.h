#ifndef WIDGET_H
#define WIDGET_H

#include "src/core/Color.h"
#include "src/core/BoxStyle.h"
#include "src/input/Mouse.h"
#include "src/core/RenderBuffer.h"
#include <string>

class Widget {
protected:
    int x, y;
    int width, height;
    bool visible;
    ColorStyle colorStyle;
    BoxStyle boxStyle;
    std::string text;
    bool needs_redraw; // Флаг необходимости перерисовки

public:
    Widget(int x, int y, int width, int height) 
        : x(x), y(y), width(width), height(height), 
          visible(true), boxStyle(BoxStyle::singleLine()), needs_redraw(true) {}
    
    virtual ~Widget() = default;
    
    // Геттеры и сеттеры
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void setPosition(int newX, int newY) { 
        if (x != newX || y != newY) {
            x = newX; 
            y = newY; 
            markDirty();
        }
    }
    
    void setSize(int newWidth, int newHeight) { 
        if (width != newWidth || height != newHeight) {
            width = newWidth; 
            height = newHeight; 
            markDirty();
        }
    }
    
    bool isVisible() const { return visible; }
    void setVisible(bool value) { 
        if (visible != value) {
            visible = value; 
            markDirty();
        }
    }
    
    void setColorStyle(const ColorStyle& style) { 
        colorStyle = style; 
        markDirty();
    }
    
    void setBoxStyle(const BoxStyle& style) { 
        boxStyle = style; 
        markDirty();
    }
    
    void setText(const std::string& newText) { 
        if (text != newText) {
            text = newText; 
            markDirty();
        }
    }
    
    const std::string& getText() const { return text; }
    
    // Методы для работы с "грязными" флагами
    void markDirty() { needs_redraw = true; }
    void markClean() { needs_redraw = false; }
    bool isDirty() const { return needs_redraw; }
    
    // Виртуальные методы
    virtual void render() = 0;
    virtual void renderToBuffer(RenderBuffer& buffer) { 
        if (needs_redraw) {
            render();
            markClean();
        }
    }
    
    virtual bool handleInput(char key) { return false; }
    
    // Методы для работы с мышью
    virtual bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) { 
        return isPointInside(mouseX, mouseY); 
    }
    
    virtual bool handleMouseMove(int mouseX, int mouseY) { return false; }
    
    // Проверка, находится ли точка внутри виджета
    virtual bool isPointInside(int pointX, int pointY) const {
        return pointX >= x && pointX < x + width &&
               pointY >= y && pointY < y + height;
    }
};

#endif