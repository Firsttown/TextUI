#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "Widget.h"
#include "src/core/Renderer.h"
#include <string>

class TextBox : public Widget {
private:
    std::string content;
    size_t cursorPos;
    bool hasFocus;
    char placeholderChar;

public:
    TextBox(int x, int y, int width) 
        : Widget(x, y, width, 3), cursorPos(0), hasFocus(false), placeholderChar('_') {}
    
    void setFocus(bool focus) { hasFocus = focus; markDirty(); }
    bool hasFocus() const { return hasFocus; }
    void setContent(const std::string& newContent) { content = newContent; cursorPos = content.length(); markDirty(); }
    const std::string& getContent() const { return content; }
    
    void render() override {
        if (!visible) return;
        
        ColorStyle textBoxStyle = colorStyle;
        if (hasFocus) {
            textBoxStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
        }
        
        // Рисуем рамку
        Renderer::drawRectangle(x, y, width, height, boxStyle, textBoxStyle);
        
        // Рисуем содержимое
        std::string displayText = content;
        if (displayText.length() >= static_cast<size_t>(width - 2)) {
            displayText = displayText.substr(0, width - 2);
        }
        
        // Добавляем placeholder символы если строка короче
        while (displayText.length() < static_cast<size_t>(width - 2)) {
            displayText += placeholderChar;
        }
        
        Renderer::drawText(x + 1, y + 1, displayText, textBoxStyle);
        
        // Рисуем курсор если есть фокус
        if (hasFocus) {
            size_t cursorDisplayPos = (cursorPos < static_cast<size_t>(width - 2)) ? cursorPos : width - 3;
            Renderer::drawChar(x + 1 + cursorDisplayPos, y + 1, '|', 
                             ColorStyle(Color::RED).setBold());
        }
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible || !needs_redraw) return;
        
        ColorStyle textBoxStyle = colorStyle;
        if (hasFocus) {
            textBoxStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
        }
        
        // Рисуем рамку в буфер
        // Горизонтальные линии
        for (int i = x + 1; i < x + width - 1; i++) {
            buffer.setStyledChar(i, y, boxStyle.horizontal,
                static_cast<int>(textBoxStyle.foreground),
                static_cast<int>(textBoxStyle.background),
                textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
            buffer.setStyledChar(i, y + height - 1, boxStyle.horizontal,
                static_cast<int>(textBoxStyle.foreground),
                static_cast<int>(textBoxStyle.background),
                textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
        }
        
        // Вертикальные линии
        for (int i = y + 1; i < y + height - 1; i++) {
            buffer.setStyledChar(x, i, boxStyle.vertical,
                static_cast<int>(textBoxStyle.foreground),
                static_cast<int>(textBoxStyle.background),
                textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
            buffer.setStyledChar(x + width - 1, i, boxStyle.vertical,
                static_cast<int>(textBoxStyle.foreground),
                static_cast<int>(textBoxStyle.background),
                textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
        }
        
        // Углы
        buffer.setStyledChar(x, y, boxStyle.top_left,
            static_cast<int>(textBoxStyle.foreground),
            static_cast<int>(textBoxStyle.background),
            textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
        buffer.setStyledChar(x + width - 1, y, boxStyle.top_right,
            static_cast<int>(textBoxStyle.foreground),
            static_cast<int>(textBoxStyle.background),
            textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
        buffer.setStyledChar(x, y + height - 1, boxStyle.bottom_left,
            static_cast<int>(textBoxStyle.foreground),
            static_cast<int>(textBoxStyle.background),
            textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
        buffer.setStyledChar(x + width - 1, y + height - 1, boxStyle.bottom_right,
            static_cast<int>(textBoxStyle.foreground),
            static_cast<int>(textBoxStyle.background),
            textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
        
        // Рисуем содержимое
        std::string displayText = content;
        if (displayText.length() >= static_cast<size_t>(width - 2)) {
            displayText = displayText.substr(0, width - 2);
        }
        
        // Добавляем placeholder символы если строка короче
        while (displayText.length() < static_cast<size_t>(width - 2)) {
            displayText += placeholderChar;
        }
        
        for (size_t i = 0; i < displayText.length() && x + 1 + i < buffer.getWidth(); i++) {
            if (y + 1 >= 0 && y + 1 < buffer.getHeight()) {
                buffer.setStyledChar(x + 1 + i, y + 1, displayText[i],
                    static_cast<int>(textBoxStyle.foreground),
                    static_cast<int>(textBoxStyle.background),
                    textBoxStyle.bold, textBoxStyle.italic, textBoxStyle.underline);
            }
        }
        
        // Рисуем курсор если есть фокус
        if (hasFocus) {
            size_t cursorDisplayPos = (cursorPos < static_cast<size_t>(width - 2)) ? cursorPos : width - 3;
            if (x + 1 + cursorDisplayPos < buffer.getWidth() && y + 1 >= 0 && y + 1 < buffer.getHeight()) {
                buffer.setStyledChar(x + 1 + cursorDisplayPos, y + 1, '|',
                    static_cast<int>(Color::RED),
                    static_cast<int>(textBoxStyle.background),
                    true, false, false); // bold
            }
        }
        
        markClean();
    }
    
    bool handleInput(char key) override {
        if (!visible || !hasFocus) return false;
        
        if (key == '\b' || key == 127) { // Backspace
            if (cursorPos > 0 && !content.empty()) {
                content.erase(cursorPos - 1, 1);
                cursorPos--;
                markDirty();
            }
            return true;
        }
        else if (key == '\t') { // Tab - передаем фокус дальше
            return false;
        }
        else if (key >= 32 && key <= 126) { // Печатаемые символы
            content.insert(cursorPos, 1, key);
            cursorPos++;
            markDirty();
            return true;
        }
        
        return false;
    }
    
    bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) override {
        if (!visible) return false;
        
        if (isPointInside(mouseX, mouseY) && button == MouseButton::LEFT && isPress) {
            hasFocus = true;
            markDirty();
            return true;
        } else if (button == MouseButton::LEFT && isPress) {
            hasFocus = false;
            markDirty();
        }
        
        return isPointInside(mouseX, mouseY);
    }
};

#endif