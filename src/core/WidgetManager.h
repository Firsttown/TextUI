#ifndef WIDGET_MANAGER_H
#define WIDGET_MANAGER_H

#include "src/widgets/Widget.h"
#include "src/window/Window.h"
#include <vector>
#include <memory>
#include <unordered_set>

// Менеджер для оптимизированного управления виджетами
class WidgetManager {
private:
    std::vector<std::unique_ptr<Window>> windows;
    std::unordered_set<Widget*> dirty_widgets; // Виджеты, требующие перерисовки
    std::unordered_set<Widget*> visible_widgets; // Видимые виджеты
    Window* focused_window;
    Window* modal_window;
    
    // Оптимизация: кэширование координат
    struct WidgetBounds {
        Widget* widget;
        int x, y, width, height;
        bool visible;
    };
    std::vector<WidgetBounds> widget_bounds_cache;
    bool bounds_cache_valid;
    
public:
    WidgetManager() : focused_window(nullptr), modal_window(nullptr), bounds_cache_valid(false) {}
    
    template<typename T, typename... Args>
    T* addWindow(Args&&... args) {
        auto window = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = window.get();
        windows.push_back(std::move(window));
        invalidateBoundsCache();
        return ptr;
    }
    
    void removeWindow(Window* window);
    
    // Оптимизированный рендеринг
    void renderOptimized(RenderBuffer& buffer);
    
    // Управление "грязными" виджетами
    void markWidgetDirty(Widget* widget);
    void markWidgetClean(Widget* widget);
    void clearDirtyWidgets();
    
    // Оптимизированный ввод
    bool handleInputOptimized(char key);
    bool handleMouseOptimized(int x, int y, MouseButton button, bool isPress);
    bool handleMouseMoveOptimized(int x, int y);
    
    // Управление фокусом
    void setFocusedWindow(Window* window);
    Window* getFocusedWindow() const { return focused_window; }
    Window* getModalWindow() const { return modal_window; }
    
    void setModalWindow(Window* window);
    void closeModalWindow(Window* window);
    
    // Оптимизация: кэширование границ
    void updateBoundsCache();
    void invalidateBoundsCache() { bounds_cache_valid = false; }
    Widget* findWidgetAt(int x, int y);
    
    const std::vector<std::unique_ptr<Window>>& getWindows() const { return windows; }
    
private:
    void renderWidgetRecursive(Widget* widget, RenderBuffer& buffer);
    Widget* findWidgetRecursive(Widget* widget, int x, int y);
};

#endif