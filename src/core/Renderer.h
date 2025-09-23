#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include "BoxStyle.h"
#include "Color.h"

class Renderer {
private:
    static void applyColor(const ColorStyle& style);
    static void resetColor();

public:
    static void drawChar(int x, int y, char ch, const ColorStyle& style = ColorStyle());
    static void drawLineHorizontal(int x, int y, int length, char ch, const ColorStyle& style = ColorStyle());
    static void drawLineVertical(int x, int y, int length, char ch, const ColorStyle& style = ColorStyle());
    static void drawRectangle(int x, int y, int width, int height, 
                            const BoxStyle& boxStyle = BoxStyle::singleLine(),
                            const ColorStyle& colorStyle = ColorStyle());
    static void drawText(int x, int y, const std::string& text, const ColorStyle& style = ColorStyle());
    static void fillArea(int x, int y, int width, int height, char ch, const ColorStyle& style = ColorStyle());
};

#endif