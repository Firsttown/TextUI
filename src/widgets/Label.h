#ifndef LABEL_H
#define LABEL_H

#include "Widget.h"
#include "../core/Renderer.h"

class Label : public Widget {
private:
    bool centered;

public:
    Label(int x, int y, const std::string& text) 
        : Widget(x, y, text.length(), 1), centered(false) {
        this->text = text;
        this->width = text.length();
    }
    
    void setCentered(bool value) { centered = value; }
    
    void render() override {
        if (!visible) return;
        
        int drawX = x;
        if (centered && width > static_cast<int>(text.length())) {
            drawX = x + (width - text.length()) / 2;
        }
        
        Renderer::drawText(drawX, y, text, colorStyle);
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible || !needs_redraw) return;
        
        int drawX = x;
        if (centered && width > static_cast<int>(text.length())) {
            drawX = x + (width - text.length()) / 2;
        }
        
        // Рендерим текст в буфер
        for (size_t i = 0; i < text.length() && drawX + i < buffer.getWidth(); i++) {
            char ch = (i < text.length()) ? text[i] : ' ';
            buffer.setStyledChar(drawX + i, y, ch,
                static_cast<int>(colorStyle.foreground),
                static_cast<int>(colorStyle.background),
                colorStyle.bold, colorStyle.italic, colorStyle.underline);
        }
        
        markClean();
    }
};

#endif