#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <vector>
#include <string>
#include <memory>

// Структура для хранения символа с атрибутами
struct RenderChar {
    char ch;
    int fg_color;
    int bg_color;
    bool bold;
    bool italic;
    bool underline;
    
    RenderChar() : ch(' '), fg_color(39), bg_color(49), bold(false), italic(false), underline(false) {}
    RenderChar(char c) : ch(c), fg_color(39), bg_color(49), bold(false), italic(false), underline(false) {}
};

// Буфер для двойной буферизации рендеринга
class RenderBuffer {
private:
    std::vector<std::vector<RenderChar>> buffer;
    std::vector<std::vector<RenderChar>> previous_buffer;
    int width, height;
    bool dirty;

public:
    RenderBuffer(int w, int h);
    
    void resize(int new_width, int new_height);
    
    void setChar(int x, int y, char ch, int fg_color = 39, int bg_color = 49);
    void setStyledChar(int x, int y, char ch, int fg_color = 39, int bg_color = 49, 
                      bool bold = false, bool italic = false, bool underline = false);
    
    RenderChar getChar(int x, int y) const;
    
    void clear();
    void fill(char ch, int fg_color = 39, int bg_color = 49);
    
    bool isDirty() const { return dirty; }
    void markClean() { dirty = false; }
    
    // Оптимизированная отрисовка - только измененные области
    void renderToTerminal();
    
    // Сравнение с предыдущим буфером для оптимизации
    bool hasChanged(int x, int y) const;
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif