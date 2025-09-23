#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "Widget.h"
#include "../core/Renderer.h"
#include <functional>

class CheckBox : public Widget {
private:
    bool checked;
    std::function<void(bool)> onToggle;
    bool hasFocus;

public:
    CheckBox(int x, int y, const std::string& text) 
        : Widget(x, y, text.length() + 4, 1), checked(false), onToggle(nullptr), hasFocus(false) {
        this->text = text;
        this->width = text.length() + 4;
    }
    
    void setChecked(bool value) { checked = value; markDirty(); }
    bool isChecked() const { return checked; }
    
    void setOnToggle(const std::function<void(bool)>& callback) {
        onToggle = callback;
    }
    
    void setFocus(bool focus) { hasFocus = focus; markDirty(); }
    bool hasFocus() const { return hasFocus; }
    
    void render() override {
        if (!visible) return;
        
        ColorStyle checkBoxStyle = colorStyle;
        if (hasFocus) {
            checkBoxStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
        }
        
        // Рисуем чекбокс
        std::string checkBoxText = "[" + std::string(checked ? "X" : " ") + "] " + text;
        Renderer::drawText(x, y, checkBoxText, checkBoxStyle);
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible || !needs_redraw) return;
        
        ColorStyle checkBoxStyle = colorStyle;
        if (hasFocus) {
            checkBoxStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
        }
        
        // Рисуем чекбокс в буфер
        std::string checkBoxText = "[" + std::string(checked ? "X" : " ") + "] " + text;
        for (size_t i = 0; i < checkBoxText.length() && x + i < buffer.getWidth(); i++) {
            if (x + i >= 0 && y >= 0 && y < buffer.getHeight()) {
                buffer.setStyledChar(x + i, y, 
                    (i < checkBoxText.length()) ? checkBoxText[i] : ' ',
                    static_cast<int>(checkBoxStyle.foreground),
                    static_cast<int>(checkBoxStyle.background),
                    checkBoxStyle.bold, checkBoxStyle.italic, checkBoxStyle.underline);
            }
        }
        
        markClean();
    }
    
    bool handleInput(char key) override {
        if (!visible || !hasFocus) return false;
        
        if (key == ' ' || key == '\n' || key == '\r') {
            checked = !checked;
            markDirty();
            if (onToggle) {
                onToggle(checked);
            }
            return true;
        }
        else if (key == '\t') {
            return false; // Передаем фокус дальше
        }
        
        return false;
    }
    
    bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) override {
        if (!visible) return false;
        
        if (isPointInside(mouseX, mouseY) && button == MouseButton::LEFT && isPress) {
            checked = !checked;
            markDirty();
            if (onToggle) {
                onToggle(checked);
            }
            return true;
        }
        
        return isPointInside(mouseX, mouseY);
    }
};

#endif