#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "Widget.h"
#include "../core/Renderer.h"
#include <string>

class ProgressBar : public Widget {
private:
    float progress; // 0.0 to 1.0
    char fillChar;
    char emptyChar;
    std::string label;

public:
    ProgressBar(int x, int y, int width) 
        : Widget(x, y, width, 3), progress(0.0f), fillChar('█'), emptyChar('░') {}
    
    void setProgress(float value) {
        progress = (value < 0.0f) ? 0.0f : (value > 1.0f) ? 1.0f : value;
        markDirty();
    }
    
    float getProgress() const { return progress; }
    
    void setFillChar(char ch) { fillChar = ch; markDirty(); }
    void setEmptyChar(char ch) { emptyChar = ch; markDirty(); }
    void setLabel(const std::string& text) { label = text; markDirty(); }
    
    void render() override {
        if (!visible) return;
        
        // Рисуем рамку
        Renderer::drawRectangle(x, y, width, height, boxStyle, colorStyle);
        
        // Рисуем прогресс
        int innerWidth = width - 2;
        int fillWidth = static_cast<int>(innerWidth * progress);
        
        std::string progressLine;
        for (int i = 0; i < innerWidth; i++) {
            progressLine += (i < fillWidth) ? fillChar : emptyChar;
        }
        
        Renderer::drawText(x + 1, y + 1, progressLine, colorStyle);
        
        // Рисуем метку или проценты
        std::string displayText = label.empty() ? 
            std::to_string(static_cast<int>(progress * 100)) + "%" : label;
        
        if (displayText.length() < static_cast<size_t>(innerWidth)) {
            int textX = x + 1 + (innerWidth - displayText.length()) / 2;
            Renderer::drawText(textX, y + 1, displayText, 
                             ColorStyle(Color::BLACK, BackgroundColor::WHITE));
        }
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible || !needs_redraw) return;
        
        // Рисуем рамку в буфер
        // Горизонтальные линии
        for (int i = x + 1; i < x + width - 1; i++) {
            buffer.setStyledChar(i, y, boxStyle.horizontal,
                static_cast<int>(colorStyle.foreground),
                static_cast<int>(colorStyle.background),
                colorStyle.bold, colorStyle.italic, colorStyle.underline);
            buffer.setStyledChar(i, y + height - 1, boxStyle.horizontal,
                static_cast<int>(colorStyle.foreground),
                static_cast<int>(colorStyle.background),
                colorStyle.bold, colorStyle.italic, colorStyle.underline);
        }
        
        // Вертикальные линии
        for (int i = y + 1; i < y + height - 1; i++) {
            buffer.setStyledChar(x, i, boxStyle.vertical,
                static_cast<int>(colorStyle.foreground),
                static_cast<int>(colorStyle.background),
                colorStyle.bold, colorStyle.italic, colorStyle.underline);
            buffer.setStyledChar(x + width - 1, i, boxStyle.vertical,
                static_cast<int>(colorStyle.foreground),
                static_cast<int>(colorStyle.background),
                colorStyle.bold, colorStyle.italic, colorStyle.underline);
        }
        
        // Углы
        buffer.setStyledChar(x, y, boxStyle.top_left,
            static_cast<int>(colorStyle.foreground),
            static_cast<int>(colorStyle.background),
            colorStyle.bold, colorStyle.italic, colorStyle.underline);
        buffer.setStyledChar(x + width - 1, y, boxStyle.top_right,
            static_cast<int>(colorStyle.foreground),
            static_cast<int>(colorStyle.background),
            colorStyle.bold, colorStyle.italic, colorStyle.underline);
        buffer.setStyledChar(x, y + height - 1, boxStyle.bottom_left,
            static_cast<int>(colorStyle.foreground),
            static_cast<int>(colorStyle.background),
            colorStyle.bold, colorStyle.italic, colorStyle.underline);
        buffer.setStyledChar(x + width - 1, y + height - 1, boxStyle.bottom_right,
            static_cast<int>(colorStyle.foreground),
            static_cast<int>(colorStyle.background),
            colorStyle.bold, colorStyle.italic, colorStyle.underline);
        
        // Рисуем прогресс
        int innerWidth = width - 2;
        int fillWidth = static_cast<int>(innerWidth * progress);
        
        for (int i = 0; i < innerWidth && x + 1 + i < buffer.getWidth(); i++) {
            char ch = (i < fillWidth) ? fillChar : emptyChar;
            if (y + 1 >= 0 && y + 1 < buffer.getHeight()) {
                buffer.setStyledChar(x + 1 + i, y + 1, ch,
                    static_cast<int>(colorStyle.foreground),
                    static_cast<int>(colorStyle.background),
                    colorStyle.bold, colorStyle.italic, colorStyle.underline);
            }
        }
        
        // Рисуем метку или проценты
        std::string displayText = label.empty() ? 
            std::to_string(static_cast<int>(progress * 100)) + "%" : label;
        
        if (displayText.length() < static_cast<size_t>(innerWidth)) {
            int textX = x + 1 + (innerWidth - displayText.length()) / 2;
            for (size_t i = 0; i < displayText.length() && textX + i < buffer.getWidth(); i++) {
                if (y + 1 >= 0 && y + 1 < buffer.getHeight()) {
                    buffer.setStyledChar(textX + i, y + 1, displayText[i],
                        static_cast<int>(Color::BLACK),
                        static_cast<int>(BackgroundColor::WHITE),
                        true, false, false); // bold
                }
            }
        }
        
        markClean();
    }
};

#endif