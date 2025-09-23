#ifndef LISTBOX_H
#define LISTBOX_H

#include "Widget.h"
#include "../core/Renderer.h"
#include <vector>
#include <functional>

class ListBox : public Widget {
private:
    std::vector<std::string> items;
    int selectedIndex;
    int scrollTop;
    std::function<void(int)> onSelectionChange;
    bool hasFocus;

public:
    ListBox(int x, int y, int width, int height) 
        : Widget(x, y, width, height), selectedIndex(-1), scrollTop(0), onSelectionChange(nullptr), hasFocus(false) {}
    
    void addItem(const std::string& item) {
        items.push_back(item);
        if (selectedIndex == -1 && !items.empty()) {
            selectedIndex = 0;
        }
        markDirty();
    }
    
    void clear() {
        items.clear();
        selectedIndex = -1;
        scrollTop = 0;
        markDirty();
    }
    
    size_t getItemCount() const { return items.size(); }
    int getSelectedIndex() const { return selectedIndex; }
    const std::string& getSelectedItem() const {
        static std::string empty;
        return (selectedIndex >= 0 && selectedIndex < static_cast<int>(items.size())) ? 
               items[selectedIndex] : empty;
    }
    
    void setSelectedIndex(int index) {
        if (index >= 0 && index < static_cast<int>(items.size())) {
            selectedIndex = index;
            // Прокручиваем, если нужно
            int visibleItems = height - 2;
            if (selectedIndex < scrollTop) {
                scrollTop = selectedIndex;
            } else if (selectedIndex >= scrollTop + visibleItems) {
                scrollTop = selectedIndex - visibleItems + 1;
            }
            markDirty();
        }
    }
    
    void setOnSelectionChange(const std::function<void(int)>& callback) {
        onSelectionChange = callback;
    }
    
    void setFocus(bool focus) { hasFocus = focus; markDirty(); }
    bool hasFocus() const { return hasFocus; }
    
    void render() override {
        if (!visible) return;
        
        ColorStyle listBoxStyle = colorStyle;
        if (hasFocus) {
            listBoxStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
        }
        
        // Рисуем рамку
        Renderer::drawRectangle(x, y, width, height, boxStyle, listBoxStyle);
        
        // Рисуем элементы
        int visibleItems = height - 2;
        int endIndex = std::min(scrollTop + visibleItems, static_cast<int>(items.size()));
        
        for (int i = scrollTop; i < endIndex; i++) {
            int yPos = y + 1 + (i - scrollTop);
            if (yPos >= y + height - 1) break;
            
            ColorStyle itemStyle = colorStyle;
            if (i == selectedIndex && hasFocus) {
                itemStyle = ColorStyle(Color::BLACK, BackgroundColor::CYAN);
            }
            
            std::string itemText = items[i];
            if (itemText.length() > static_cast<size_t>(width - 2)) {
                itemText = itemText.substr(0, width - 5) + "...";
            }
            
            Renderer::drawText(x + 1, yPos, itemText, itemStyle);
        }
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible || !needs_redraw) return;
        
        ColorStyle listBoxStyle = colorStyle;
        if (hasFocus) {
            listBoxStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
        }
        
        // Рисуем рамку в буфер
        // Горизонтальные линии
        for (int i = x + 1; i < x + width - 1; i++) {
            buffer.setStyledChar(i, y, boxStyle.horizontal,
                static_cast<int>(listBoxStyle.foreground),
                static_cast<int>(listBoxStyle.background),
                listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
            buffer.setStyledChar(i, y + height - 1, boxStyle.horizontal,
                static_cast<int>(listBoxStyle.foreground),
                static_cast<int>(listBoxStyle.background),
                listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
        }
        
        // Вертикальные линии
        for (int i = y + 1; i < y + height - 1; i++) {
            buffer.setStyledChar(x, i, boxStyle.vertical,
                static_cast<int>(listBoxStyle.foreground),
                static_cast<int>(listBoxStyle.background),
                listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
            buffer.setStyledChar(x + width - 1, i, boxStyle.vertical,
                static_cast<int>(listBoxStyle.foreground),
                static_cast<int>(listBoxStyle.background),
                listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
        }
        
        // Углы
        buffer.setStyledChar(x, y, boxStyle.top_left,
            static_cast<int>(listBoxStyle.foreground),
            static_cast<int>(listBoxStyle.background),
            listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
        buffer.setStyledChar(x + width - 1, y, boxStyle.top_right,
            static_cast<int>(listBoxStyle.foreground),
            static_cast<int>(listBoxStyle.background),
            listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
        buffer.setStyledChar(x, y + height - 1, boxStyle.bottom_left,
            static_cast<int>(listBoxStyle.foreground),
            static_cast<int>(listBoxStyle.background),
            listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
        buffer.setStyledChar(x + width - 1, y + height - 1, boxStyle.bottom_right,
            static_cast<int>(listBoxStyle.foreground),
            static_cast<int>(listBoxStyle.background),
            listBoxStyle.bold, listBoxStyle.italic, listBoxStyle.underline);
        
        // Рисуем элементы
        int visibleItems = height - 2;
        int endIndex = std::min(scrollTop + visibleItems, static_cast<int>(items.size()));
        
        for (int i = scrollTop; i < endIndex; i++) {
            int yPos = y + 1 + (i - scrollTop);
            if (yPos >= y + height - 1) break;
            
            ColorStyle itemStyle = colorStyle;
            if (i == selectedIndex && hasFocus) {
                itemStyle = ColorStyle(Color::BLACK, BackgroundColor::CYAN);
            }
            
            std::string itemText = items[i];
            if (itemText.length() > static_cast<size_t>(width - 2)) {
                itemText = itemText.substr(0, width - 5) + "...";
            }
            
            // Заполняем строку пробелами
            for (int j = 0; j < width - 2; j++) {
                char ch = (j < static_cast<int>(itemText.length())) ? itemText[j] : ' ';
                if (x + 1 + j >= 0 && x + 1 + j < buffer.getWidth() && yPos >= 0 && yPos < buffer.getHeight()) {
                    buffer.setStyledChar(x + 1 + j, yPos, ch,
                        static_cast<int>(itemStyle.foreground),
                        static_cast<int>(itemStyle.background),
                        itemStyle.bold, itemStyle.italic, itemStyle.underline);
                }
            }
        }
        
        markClean();
    }
    
    bool handleInput(char key) override {
        if (!visible || !hasFocus) return false;
        
        int itemCount = static_cast<int>(items.size());
        if (itemCount == 0) return false;
        
        switch (key) {
            case 'w':
            case 'W':
            case 72: // Up arrow (если поддерживается)
                if (selectedIndex > 0) {
                    selectedIndex--;
                    if (selectedIndex < scrollTop) {
                        scrollTop = selectedIndex;
                    }
                    if (onSelectionChange) {
                        onSelectionChange(selectedIndex);
                    }
                    markDirty();
                    return true;
                }
                break;
                
            case 's':
            case 'S':
            case 80: // Down arrow (если поддерживается)
                if (selectedIndex < itemCount - 1) {
                    selectedIndex++;
                    int visibleItems = height - 2;
                    if (selectedIndex >= scrollTop + visibleItems) {
                        scrollTop = selectedIndex - visibleItems + 1;
                    }
                    if (onSelectionChange) {
                        onSelectionChange(selectedIndex);
                    }
                    markDirty();
                    return true;
                }
                break;
                
            case '\n':
            case '\r':
                if (onSelectionChange && selectedIndex >= 0) {
                    onSelectionChange(selectedIndex);
                }
                return true;
                
            case '\t':
                return false; // Передаем фокус дальше
        }
        
        return false;
    }
    
    bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) override {
        if (!visible || !hasFocus) return false;
        
        if (isPointInside(mouseX, mouseY) && button == MouseButton::LEFT && isPress) {
            int relativeY = mouseY - (y + 1);
            if (relativeY >= 0 && relativeY < height - 2) {
                int itemIndex = scrollTop + relativeY;
                if (itemIndex >= 0 && itemIndex < static_cast<int>(items.size())) {
                    selectedIndex = itemIndex;
                    if (onSelectionChange) {
                        onSelectionChange(selectedIndex);
                    }
                    markDirty();
                    return true;
                }
            }
            return true;
        }
        
        return isPointInside(mouseX, mouseY);
    }
};

#endif