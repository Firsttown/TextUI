#include "RenderBuffer.h"
#include "src/platform/Platform.h"
#include <algorithm>
#include <iostream>

RenderBuffer::RenderBuffer(int w, int h) : width(w), height(h), dirty(false) {
    buffer.resize(height, std::vector<RenderChar>(width));
    previous_buffer.resize(height, std::vector<RenderChar>(width));
}

void RenderBuffer::resize(int new_width, int new_height) {
    if (new_width == width && new_height == height) return;
    
    width = new_width;
    height = new_height;
    
    buffer.resize(height);
    for (auto& row : buffer) {
        row.resize(width);
    }
    
    previous_buffer.resize(height);
    for (auto& row : previous_buffer) {
        row.resize(width);
    }
    
    dirty = true;
}

void RenderBuffer::setChar(int x, int y, char ch, int fg_color, int bg_color) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    
    auto& cell = buffer[y][x];
    if (cell.ch != ch || cell.fg_color != fg_color || cell.bg_color != bg_color) {
        cell.ch = ch;
        cell.fg_color = fg_color;
        cell.bg_color = bg_color;
        cell.bold = false;
        cell.italic = false;
        cell.underline = false;
        dirty = true;
    }
}

void RenderBuffer::setStyledChar(int x, int y, char ch, int fg_color, int bg_color, 
                                bool bold, bool italic, bool underline) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    
    auto& cell = buffer[y][x];
    if (cell.ch != ch || cell.fg_color != fg_color || cell.bg_color != bg_color ||
        cell.bold != bold || cell.italic != italic || cell.underline != underline) {
        cell.ch = ch;
        cell.fg_color = fg_color;
        cell.bg_color = bg_color;
        cell.bold = bold;
        cell.italic = italic;
        cell.underline = underline;
        dirty = true;
    }
}

RenderChar RenderBuffer::getChar(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return RenderChar();
    }
    return buffer[y][x];
}

void RenderBuffer::clear() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            buffer[y][x] = RenderChar(' ');
        }
    }
    dirty = true;
}

void RenderBuffer::fill(char ch, int fg_color, int bg_color) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            setChar(x, y, ch, fg_color, bg_color);
        }
    }
}

void RenderBuffer::renderToTerminal() {
    if (!dirty) return;
    
    // Оптимизированная отрисовка - только измененные символы
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (hasChanged(x, y)) {
                const auto& cell = buffer[y][x];
                
                // Устанавливаем позицию курсора
                Platform::moveCursor(x, y);
                
                // Формируем ANSI коды
                std::string ansi_codes;
                bool has_formatting = false;
                
                if (cell.fg_color != 39) {
                    ansi_codes += "\033[" + std::to_string(cell.fg_color) + "m";
                    has_formatting = true;
                }
                
                if (cell.bg_color != 49) {
                    ansi_codes += "\033[" + std::to_string(cell.bg_color) + "m";
                    has_formatting = true;
                }
                
                if (cell.bold) {
                    ansi_codes += "\033[1m";
                    has_formatting = true;
                }
                
                if (cell.italic) {
                    ansi_codes += "\033[3m";
                    has_formatting = true;
                }
                
                if (cell.underline) {
                    ansi_codes += "\033[4m";
                    has_formatting = true;
                }
                
                // Выводим символ
                if (has_formatting) {
                    std::cout << ansi_codes << cell.ch << "\033[0m";
                } else {
                    std::cout << cell.ch;
                }
                
                // Сохраняем в предыдущий буфер
                previous_buffer[y][x] = cell;
            }
        }
    }
    
    std::cout.flush();
    markClean();
}

bool RenderBuffer::hasChanged(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    const auto& current = buffer[y][x];
    const auto& previous = previous_buffer[y][x];
    
    return current.ch != previous.ch ||
           current.fg_color != previous.fg_color ||
           current.bg_color != previous.bg_color ||
           current.bold != previous.bold ||
           current.italic != previous.italic ||
           current.underline != previous.underline;
}