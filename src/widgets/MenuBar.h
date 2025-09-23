#ifndef MENUBAR_H
#define MENUBAR_H

#include "Widget.h"
#include "../core/Renderer.h"
#include <vector>
#include <functional>

struct MenuItem {
    std::string text;
    std::function<void()> action;
    std::vector<MenuItem> subItems;
    
    MenuItem(const std::string& t) : text(t) {}
    MenuItem(const std::string& t, const std::function<void()>& a) : text(t), action(a) {}
};

class MenuBar : public Widget {
private:
    std::vector<MenuItem> items;
    int selectedMenuIndex;
    bool menuOpen;
    int openSubMenuIndex;

public:
    MenuBar(int x, int y, int width) 
        : Widget(x, y, width, 1), selectedMenuIndex(-1), menuOpen(false), openSubMenuIndex(-1) {}
    
    void addMenuItem(const MenuItem& item) {
        items.push_back(item);
        markDirty();
    }
    
    void render() override {
        if (!visible) return;
        
        // Рисуем фон меню
        Renderer::fillArea(x, y, width, 1, ' ', 
                          ColorStyle(Color::BLACK, BackgroundColor::WHITE).setBold());
        
        // Рисуем пункты меню
        int currentX = x;
        for (size_t i = 0; i < items.size(); i++) {
            ColorStyle itemStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
            if (static_cast<int>(i) == selectedMenuIndex) {
                itemStyle = ColorStyle(Color::WHITE, BackgroundColor::BLUE).setBold();
            }
            
            std::string itemText = " " + items[i].text + " ";
            Renderer::drawText(currentX, y, itemText, itemStyle);
            currentX += itemText.length();
        }
        
        // Рисуем выпадающее меню, если открыто
        if (menuOpen && selectedMenuIndex >= 0 && 
            selectedMenuIndex < static_cast<int>(items.size()) &&
            !items[selectedMenuIndex].subItems.empty()) {
            
            const auto& subItems = items[selectedMenuIndex].subItems;
            int menuX = getXForMenu(selectedMenuIndex);
            int menuY = y + 1;
            int menuHeight = static_cast<int>(subItems.size()) + 2;
            int menuWidth = getMaxSubItemWidth(subItems) + 4;
            
            // Рисуем рамку выпадающего меню
            Renderer::drawRectangle(menuX, menuY, menuWidth, menuHeight, 
                                  BoxStyle::singleLine(),
                                  ColorStyle(Color::BLACK, BackgroundColor::WHITE));
            
            // Рисуем пункты подменю
            for (size_t i = 0; i < subItems.size(); i++) {
                ColorStyle subItemStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
                if (static_cast<int>(i) == openSubMenuIndex) {
                    subItemStyle = ColorStyle(Color::WHITE, BackgroundColor::BLUE);
                }
                
                std::string itemText = " " + subItems[i].text;
                if (itemText.length() > static_cast<size_t>(menuWidth - 2)) {
                    itemText = itemText.substr(0, menuWidth - 5) + "...";
                }
                
                Renderer::drawText(menuX + 1, menuY + 1 + i, itemText, subItemStyle);
            }
        }
    }
    
    void renderToBuffer(RenderBuffer& buffer) override {
        if (!visible || !needs_redraw) return;
        
        // Рисуем фон меню в буфер
        for (int i = 0; i < width && x + i < buffer.getWidth(); i++) {
            if (y >= 0 && y < buffer.getHeight()) {
                buffer.setStyledChar(x + i, y, ' ',
                    static_cast<int>(Color::BLACK),
                    static_cast<int>(BackgroundColor::WHITE),
                    true, false, false); // bold
            }
        }
        
        // Рисуем пункты меню
        int currentX = x;
        for (size_t i = 0; i < items.size(); i++) {
            ColorStyle itemStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
            if (static_cast<int>(i) == selectedMenuIndex) {
                itemStyle = ColorStyle(Color::WHITE, BackgroundColor::BLUE).setBold();
            }
            
            std::string itemText = " " + items[i].text + " ";
            for (size_t j = 0; j < itemText.length() && currentX + j < buffer.getWidth(); j++) {
                if (y >= 0 && y < buffer.getHeight()) {
                    buffer.setStyledChar(currentX + j, y, 
                        (j < itemText.length()) ? itemText[j] : ' ',
                        static_cast<int>(itemStyle.foreground),
                        static_cast<int>(itemStyle.background),
                        itemStyle.bold, itemStyle.italic, itemStyle.underline);
                }
            }
            currentX += itemText.length();
        }
        
        // Рисуем выпадающее меню, если открыто
        if (menuOpen && selectedMenuIndex >= 0 && 
            selectedMenuIndex < static_cast<int>(items.size()) &&
            !items[selectedMenuIndex].subItems.empty()) {
            
            const auto& subItems = items[selectedMenuIndex].subItems;
            int menuX = getXForMenu(selectedMenuIndex);
            int menuY = y + 1;
            int menuHeight = static_cast<int>(subItems.size()) + 2;
            int menuWidth = getMaxSubItemWidth(subItems) + 4;
            
            // Рисуем рамку выпадающего меню
            // Горизонтальные линии
            for (int i = menuX + 1; i < menuX + menuWidth - 1 && i < buffer.getWidth(); i++) {
                if (menuY >= 0 && menuY < buffer.getHeight()) {
                    buffer.setStyledChar(i, menuY, '-',
                        static_cast<int>(Color::BLACK),
                        static_cast<int>(BackgroundColor::WHITE),
                        false, false, false);
                }
                if (menuY + menuHeight - 1 >= 0 && menuY + menuHeight - 1 < buffer.getHeight()) {
                    buffer.setStyledChar(i, menuY + menuHeight - 1, '-',
                        static_cast<int>(Color::BLACK),
                        static_cast<int>(BackgroundColor::WHITE),
                        false, false, false);
                }
            }
            
            // Вертикальные линии
            for (int i = menuY + 1; i < menuY + menuHeight - 1 && i < buffer.getHeight(); i++) {
                if (menuX >= 0 && menuX < buffer.getWidth()) {
                    buffer.setStyledChar(menuX, i, '|',
                        static_cast<int>(Color::BLACK),
                        static_cast<int>(BackgroundColor::WHITE),
                        false, false, false);
                }
                if (menuX + menuWidth - 1 >= 0 && menuX + menuWidth - 1 < buffer.getWidth()) {
                    buffer.setStyledChar(menuX + menuWidth - 1, i, '|',
                        static_cast<int>(Color::BLACK),
                        static_cast<int>(BackgroundColor::WHITE),
                        false, false, false);
                }
            }
            
            // Углы
            if (menuX >= 0 && menuX < buffer.getWidth() && menuY >= 0 && menuY < buffer.getHeight()) {
                buffer.setStyledChar(menuX, menuY, '+',
                    static_cast<int>(Color::BLACK),
                    static_cast<int>(BackgroundColor::WHITE),
                    false, false, false);
            }
            if (menuX + menuWidth - 1 >= 0 && menuX + menuWidth - 1 < buffer.getWidth() && 
                menuY >= 0 && menuY < buffer.getHeight()) {
                buffer.setStyledChar(menuX + menuWidth - 1, menuY, '+',
                    static_cast<int>(Color::BLACK),
                    static_cast<int>(BackgroundColor::WHITE),
                    false, false, false);
            }
            if (menuX >= 0 && menuX < buffer.getWidth() && 
                menuY + menuHeight - 1 >= 0 && menuY + menuHeight - 1 < buffer.getHeight()) {
                buffer.setStyledChar(menuX, menuY + menuHeight - 1, '+',
                    static_cast<int>(Color::BLACK),
                    static_cast<int>(BackgroundColor::WHITE),
                    false, false, false);
            }
            if (menuX + menuWidth - 1 >= 0 && menuX + menuWidth - 1 < buffer.getWidth() && 
                menuY + menuHeight - 1 >= 0 && menuY + menuHeight - 1 < buffer.getHeight()) {
                buffer.setStyledChar(menuX + menuWidth - 1, menuY + menuHeight - 1, '+',
                    static_cast<int>(Color::BLACK),
                    static_cast<int>(BackgroundColor::WHITE),
                    false, false, false);
            }
            
            // Рисуем пункты подменю
            for (size_t i = 0; i < subItems.size(); i++) {
                ColorStyle subItemStyle = ColorStyle(Color::BLACK, BackgroundColor::WHITE);
                if (static_cast<int>(i) == openSubMenuIndex) {
                    subItemStyle = ColorStyle(Color::WHITE, BackgroundColor::BLUE);
                }
                
                std::string itemText = " " + subItems[i].text;
                if (itemText.length() > static_cast<size_t>(menuWidth - 2)) {
                    itemText = itemText.substr(0, menuWidth - 5) + "...";
                }
                
                for (size_t j = 0; j < itemText.length() && menuX + 1 + j < buffer.getWidth(); j++) {
                    if (menuY + 1 + i >= 0 && menuY + 1 + i < buffer.getHeight()) {
                        buffer.setStyledChar(menuX + 1 + j, menuY + 1 + i,
                            (j < itemText.length()) ? itemText[j] : ' ',
                            static_cast<int>(subItemStyle.foreground),
                            static_cast<int>(subItemStyle.background),
                            subItemStyle.bold, subItemStyle.italic, subItemStyle.underline);
                    }
                }
            }
        }
        
        markClean();
    }
    
    bool handleInput(char key) override {
        if (!visible) return false;
        
        switch (key) {
            case '\t':
                return false; // Передаем фокус дальше
                
            case 'a':
            case 'A':
            case 75: // Left arrow
                if (!menuOpen) {
                    selectedMenuIndex = (selectedMenuIndex <= 0) ? 
                        static_cast<int>(items.size()) - 1 : selectedMenuIndex - 1;
                } else if (openSubMenuIndex > 0) {
                    openSubMenuIndex--;
                }
                markDirty();
                return true;
                
            case 'd':
            case 'D':
            case 77: // Right arrow
                if (!menuOpen) {
                    selectedMenuIndex = (selectedMenuIndex >= static_cast<int>(items.size()) - 1) ? 
                        0 : selectedMenuIndex + 1;
                } else if (openSubMenuIndex < static_cast<int>(items[selectedMenuIndex].subItems.size()) - 1) {
                    openSubMenuIndex++;
                }
                markDirty();
                return true;
                
            case 's':
            case 'S':
            case 80: // Down arrow
                if (!menuOpen && selectedMenuIndex >= 0) {
                    menuOpen = true;
                    openSubMenuIndex = 0;
                } else if (menuOpen && openSubMenuIndex < static_cast<int>(items[selectedMenuIndex].subItems.size()) - 1) {
                    openSubMenuIndex++;
                }
                markDirty();
                return true;
                
            case 'w':
            case 'W':
            case 72: // Up arrow
                if (menuOpen && openSubMenuIndex > 0) {
                    openSubMenuIndex--;
                }
                markDirty();
                return true;
                
            case '\n':
            case '\r':
            case ' ':
                if (!menuOpen && selectedMenuIndex >= 0) {
                    menuOpen = true;
                    openSubMenuIndex = 0;
                } else if (menuOpen) {
                    if (openSubMenuIndex >= 0 && 
                        openSubMenuIndex < static_cast<int>(items[selectedMenuIndex].subItems.size())) {
                        auto& selectedItem = items[selectedMenuIndex].subItems[openSubMenuIndex];
                        if (selectedItem.action) {
                            selectedItem.action();
                        }
                    }
                    menuOpen = false;
                    openSubMenuIndex = -1;
                } else if (selectedMenuIndex >= 0 && items[selectedMenuIndex].action) {
                    items[selectedMenuIndex].action();
                }
                markDirty();
                return true;
                
            case 27: // Escape
                menuOpen = false;
                openSubMenuIndex = -1;
                markDirty();
                return true;
        }
        
        return false;
    }
    
    bool handleMouse(int mouseX, int mouseY, MouseButton button, bool isPress) override {
        if (!visible) return false;
        
        // Проверяем клик по пунктам меню
        if (mouseY == y && button == MouseButton::LEFT && isPress) {
            int currentX = x;
            for (size_t i = 0; i < items.size(); i++) {
                std::string itemText = " " + items[i].text + " ";
                if (mouseX >= currentX && mouseX < currentX + static_cast<int>(itemText.length())) {
                    selectedMenuIndex = static_cast<int>(i);
                    menuOpen = true;
                    openSubMenuIndex = 0;
                    markDirty();
                    return true;
                }
                currentX += itemText.length();
            }
        }
        
        // Проверяем клик по выпадающему меню
        if (menuOpen && selectedMenuIndex >= 0 && 
            selectedMenuIndex < static_cast<int>(items.size()) &&
            !items[selectedMenuIndex].subItems.empty() && 
            button == MouseButton::LEFT && isPress) {
            
            const auto& subItems = items[selectedMenuIndex].subItems;
            int menuX = getXForMenu(selectedMenuIndex);
            int menuY = y + 1;
            int menuHeight = static_cast<int>(subItems.size()) + 2;
            int menuWidth = getMaxSubItemWidth(subItems) + 4;
            
            if (mouseX >= menuX && mouseX < menuX + menuWidth &&
                mouseY >= menuY && mouseY < menuY + menuHeight) {
                
                int relativeY = mouseY - (menuY + 1);
                if (relativeY >= 0 && relativeY < static_cast<int>(subItems.size())) {
                    openSubMenuIndex = relativeY;
                    auto& selectedItem = subItems[openSubMenuIndex];
                    if (selectedItem.action) {
                        selectedItem.action();
                    }
                    menuOpen = false;
                    openSubMenuIndex = -1;
                    markDirty();
                    return true;
                }
            }
        }
        
        return isPointInside(mouseX, mouseY);
    }
    
    void closeMenu() {
        menuOpen = false;
        openSubMenuIndex = -1;
        markDirty();
    }
    
private:
    int getXForMenu(int menuIndex) {
        int currentX = x;
        for (int i = 0; i < menuIndex; i++) {
            currentX += items[i].text.length() + 2;
        }
        return currentX;
    }
    
    int getMaxSubItemWidth(const std::vector<MenuItem>& subItems) {
        size_t maxWidth = 0;
        for (const auto& item : subItems) {
            maxWidth = std::max(maxWidth, item.text.length());
        }
        return static_cast<int>(maxWidth);
    }
};

#endif