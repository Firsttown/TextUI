#ifndef BUTTON_H
#define BUTTON_H

#include "Widget.h"
#include "../core/Renderer.h"
#include <functional>

class Button : public Widget {
private:
    std::function<void()> onClick;
    bool isPressed;
    bool hasFocus;
    bool isHovered;

public:
    Button(int x, int y, int width, const std::string& text) 
        : Widget(x, y, width, 3), onClick(nullptr), isPressed(false), hasFocus(false), isHovered(false) {
        this->text = text;
        if (static_cast<int>(text.length()) > width - 2) {
            this->width = text.length() + 2;
        }
    }
    
    void setOnClick(const std::function<void()>& callback) {
        onClick = callback;
    }
    
    void setFocus(bool focus) { hasFocus = focus; }
    bool hasFocus() const { return hasFocus; }
    
    void render() override {
        if (!visible) return;
        
        ColorStyle buttonStyle = colorStyle;
        if (hasFocus || isHovered) {
            buttonStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE).setBold();
        } else if (isPressed) {
            buttonStyle = ColorStyle(Color::WHITE, BackgroundColor::BLUE);
        }
        
        // Рисуем рамку кнопки
        Renderer::drawRectangle(x, y, width, height, boxStyle, buttonStyle);
        
        // Рисуем текст по центру
        int textX = x + (width - text.length()) / 2;
        int textY = y + height / 2;
        Renderer::drawText(textX, textY, text, buttonStyle);
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible || !needs_redraw) return;
        
        ColorStyle buttonStyle = colorStyle;
        if (hasFocus || isHovered) {
            buttonStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE).setBold();
        } else if (isPressed) {
            buttonStyle = ColorStyle(Color::WHITE, BackgroundColor::BLUE);
        }
        
        // Рисуем рамку кнопки в буфер
        // Верхняя линия
        for (int i = x + 1; i < x + width - 1; i++) {
            buffer.setStyledChar(i, y, boxStyle.horizontal,
                static_cast<int>(buttonStyle.foreground),
                static_cast<int>(buttonStyle.background),
                buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        }
        
        // Нижняя линия
        for (int i = x + 1; i < x + width - 1; i++) {
            buffer.setStyledChar(i, y + height - 1, boxStyle.horizontal,
                static_cast<int>(buttonStyle.foreground),
                static_cast<int>(buttonStyle.background),
                buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        }
        
        // Левые и правые линии
        for (int i = y + 1; i < y + height - 1; i++) {
            buffer.setStyledChar(x, i, boxStyle.vertical,
                static_cast<int>(buttonStyle.foreground),
                static_cast<int>(buttonStyle.background),
                buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
            buffer.setStyledChar(x + width - 1, i, boxStyle.vertical,
                static_cast<int>(buttonStyle.foreground),
                static_cast<int>(buttonStyle.background),
                buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        }
        
        // Углы
        buffer.setStyledChar(x, y, boxStyle.top_left,
            static_cast<int>(buttonStyle.foreground),
            static_cast<int>(buttonStyle.background),
            buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        buffer.setStyledChar(x + width - 1, y, boxStyle.top_right,
            static_cast<int>(buttonStyle.foreground),
            static_cast<int>(buttonStyle.background),
            buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        buffer.setStyledChar(x, y + height - 1, boxStyle.bottom_left,
            static_cast<int>(buttonStyle.foreground),
            static_cast<int>(buttonStyle.background),
            buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        buffer.setStyledChar(x + width - 1, y + height - 1, boxStyle.bottom_right,
            static_cast<int>(buttonStyle.foreground),
            static_cast<int>(buttonStyle.background),
            buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        
        // Рисуем текст по центру
        int textX = x + (width - text.length()) / 2;
        int textY = y + height / 2;
        for (size_t i = 0; i < text.length() && textX + i < buffer.getWidth(); i++) {
            buffer.setStyledChar(textX + i, textY, text[i],
                static_cast<int>(buttonStyle.foreground),
                static_cast<int>(buttonStyle.background),
                buttonStyle.bold, buttonStyle.italic, buttonStyle.underline);
        }
        
        markClean();
    }
    
    bool handleInput(char key) override {
        if (!visible || !hasFocus) return false;
        
        switch (key) {
            case ' ':
            case '\n':
            case '\r':
                triggerClick();
                return true;
            case '\t': // Tab - передаем фокус дальше
                return false;
        }
        return false;
    }
    
    bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) override {
        if (!visible) return false;
        
        if (isPointInside(mouseX, mouseY)) {
            if (button == MouseButton::LEFT) {
                if (isPress) {
                    isPressed = true;
                    isHovered = true;
                } else {
                    if (isPressed) {
                        triggerClick();
                    }
                    isPressed = false;
                }
                return true;
            }
            return true; // Мышь над кнопкой
        } else {
            isPressed = false;
            isHovered = false;
        }
        
        return false;
    }
    
    bool handleMouseMove(int mouseX, int mouseY) override {
        if (!visible) return false;
        
        if (isPointInside(mouseX, mouseY)) {
            isHovered = true;
            return true;
        } else {
            isHovered = false;
            isPressed = false;
        }
        
        return false;
    }
    
private:
    void triggerClick() {
        isPressed = false;
        if (onClick) {
            onClick();
        }
    }
};

#endif