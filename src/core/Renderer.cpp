#include "Renderer.h"
#include "../platform/Platform.h"
#include <iostream>
#include <sstream>

void Renderer::applyColor(const ColorStyle& style) {
    std::ostringstream colorCode;
    colorCode << "\033[";
    
    bool first = true;
    
    // Фоновый цвет
    if (style.background != BackgroundColor::DEFAULT) {
        if (!first) colorCode << ";";
        colorCode << static_cast<int>(style.background);
        first = false;
    }
    
    // Основной цвет
    if (style.foreground != Color::DEFAULT) {
        if (!first) colorCode << ";";
        colorCode << static_cast<int>(style.foreground);
        first = false;
    }
    
    // Жирный текст
    if (style.bold) {
        if (!first) colorCode << ";";
        colorCode << "1";
        first = false;
    }
    
    // Курсив
    if (style.italic) {
        if (!first) colorCode << ";";
        colorCode << "3";
        first = false;
    }
    
    // Подчеркивание
    if (style.underline) {
        if (!first) colorCode << ";";
        colorCode << "4";
        first = false;
    }
    
    colorCode << "m";
    std::cout << colorCode.str();
}

void Renderer::resetColor() {
    std::cout << "\033[0m";
}

void Renderer::drawChar(int x, int y, char ch, const ColorStyle& style) {
    Platform::moveCursor(x, y);
    applyColor(style);
    std::cout << ch;
    resetColor();
}

void Renderer::drawLineHorizontal(int x, int y, int length, char ch, const ColorStyle& style) {
    Platform::moveCursor(x, y);
    applyColor(style);
    for (int i = 0; i < length; i++) {
        std::cout << ch;
    }
    resetColor();
}

void Renderer::drawLineVertical(int x, int y, int length, char ch, const ColorStyle& style) {
    applyColor(style);
    for (int i = 0; i < length; i++) {
        drawChar(x, y + i, ch, ColorStyle()); // Color уже применен
    }
    resetColor();
}

void Renderer::drawRectangle(int x, int y, int width, int height, 
                           const BoxStyle& boxStyle, const ColorStyle& colorStyle) {
    if (width < 2 || height < 2) return;
    
    applyColor(colorStyle);
    
    // Горизонтальные линии
    drawLineHorizontal(x + 1, y, width - 2, boxStyle.horizontal, ColorStyle());
    drawLineHorizontal(x + 1, y + height - 1, width - 2, boxStyle.horizontal, ColorStyle());
    
    // Вертикальные линии
    drawLineVertical(x, y + 1, height - 2, boxStyle.vertical, ColorStyle());
    drawLineVertical(x + width - 1, y + 1, height - 2, boxStyle.vertical, ColorStyle());
    
    // Углы
    drawChar(x, y, boxStyle.top_left, ColorStyle());
    drawChar(x + width - 1, y, boxStyle.top_right, ColorStyle());
    drawChar(x, y + height - 1, boxStyle.bottom_left, ColorStyle());
    drawChar(x + width - 1, y + height - 1, boxStyle.bottom_right, ColorStyle());
    
    resetColor();
}

void Renderer::drawText(int x, int y, const std::string& text, const ColorStyle& style) {
    Platform::moveCursor(x, y);
    applyColor(style);
    std::cout << text;
    resetColor();
}

void Renderer::fillArea(int x, int y, int width, int height, char ch, const ColorStyle& style) {
    applyColor(style);
    for (int row = 0; row < height; row++) {
        Platform::moveCursor(x, y + row);
        for (int col = 0; col < width; col++) {
            std::cout << ch;
        }
    }
    resetColor();
}