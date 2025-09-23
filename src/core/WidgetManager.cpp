#include "WidgetManager.h"
#include "../core/RenderBuffer.h"
#include <algorithm>

void WidgetManager::removeWindow(Window* window) {
    auto it = std::find_if(windows.begin(), windows.end(),
        [window](const std::unique_ptr<Window>& w) { return w.get() == window; });
    if (it != windows.end()) {
        windows.erase(it);
        invalidateBoundsCache();
    }

    if (focused_window == window) {
        focused_window = nullptr;
    }

    if (modal_window == window) {
        modal_window = nullptr;
    }
}

void WidgetManager::renderOptimized(RenderBuffer& buffer) {
    // Очищаем буфер
    buffer.clear();

    // Рендерим все окна
    for (auto& window : windows) {
        if (window->isVisible()) {
            renderWidgetRecursive(window.get(), buffer);
        }
    }

    // Если есть модальное окно, рендерим его поверх
    if (modal_window && modal_window->isVisible()) {
        renderWidgetRecursive(modal_window, buffer);
    }
}

void WidgetManager::renderWidgetRecursive(Widget* widget, RenderBuffer& buffer) {
    if (!widget || !widget->isVisible()) return;

    // Рендерим сам виджет
    widget->renderToBuffer(buffer);

    // Если это контейнер, рендерим дочерние элементы
    if (auto* container = dynamic_cast<Container*>(widget)) {
        for (auto& child : container->getChildren()) {
            if (child && child->isVisible()) {
                renderWidgetRecursive(child.get(), buffer);
            }
        }
    }

    // Если это окно, рендерим дочерние окна
    if (auto* window = dynamic_cast<Window*>(widget)) {
        for (auto* childWindow : window->getChildWindows()) {
            if (childWindow && childWindow->isVisible()) {
                renderWidgetRecursive(childWindow, buffer);
            }
        }
    }
}

void WidgetManager::markWidgetDirty(Widget* widget) {
    if (widget) {
        dirty_widgets.insert(widget);
    }
}

void WidgetManager::markWidgetClean(Widget* widget) {
    if (widget) {
        dirty_widgets.erase(widget);
    }
}

void WidgetManager::clearDirtyWidgets() {
    dirty_widgets.clear();
}

bool WidgetManager::handleInputOptimized(char key) {
    // Если есть модальное окно, передаем ввод только ему
    if (modal_window && modal_window->isVisible()) {
        bool handled = modal_window->handleInput(key);
        if (handled) {
            markWidgetDirty(modal_window);
        }
        return handled;
    }

    // Передаем активному окну
    if (focused_window && focused_window->isVisible()) {
        bool handled = focused_window->handleInput(key);
        if (handled) {
            markWidgetDirty(focused_window);
        }
        return handled;
    }

    return false;
}

bool WidgetManager::handleMouseOptimized(int x, int y, MouseButton button, bool isPress) {
    // Если есть модальное окно, обрабатываем только его
    if (modal_window && modal_window->isVisible()) {
        if (modal_window->isPointInside(x, y)) {
            bool handled = modal_window->handleMouse(x, y, button, isPress);
            if (handled) {
                markWidgetDirty(modal_window);
            }
            return handled;
        } else if (button == MouseButton::LEFT && isPress) {
            return true; // Клик вне модального окна игнорируется
        }
        return false;
    }

    // Используем кэш границ для быстрого поиска
    if (!bounds_cache_valid) {
        updateBoundsCache();
    }

    // Поиск виджета по координатам (с конца - верхние первыми)
    for (auto it = widget_bounds_cache.rbegin(); it != widget_bounds_cache.rend(); ++it) {
        const auto& bounds = *it;
        if (bounds.visible &&
            x >= bounds.x && x < bounds.x + bounds.width &&
            y >= bounds.y && y < bounds.y + bounds.height) {

            Widget* widget = bounds.widget;
            bool handled = widget->handleMouse(x, y, button, isPress);
            if (handled) {
                markWidgetDirty(widget);

                // Устанавливаем фокус, если это окно
                if (auto* window = dynamic_cast<Window*>(widget)) {
                    setFocusedWindow(window);
                }
            }
            return handled;
        }
    }

    return false;
}

bool WidgetManager::handleMouseMoveOptimized(int x, int y) {
    bool handled = false;

    // Передаем движение активному окну
    if (focused_window && focused_window->isVisible()) {
        if (focused_window->handleMouseMove(x, y)) {
            markWidgetDirty(focused_window);
            handled = true;
        }
    }

    // Передаем всем окнам для hover эффектов
    for (auto& window : windows) {
        if (window->isVisible() && window.get() != focused_window) {
            if (window->handleMouseMove(x, y)) {
                markWidgetDirty(window.get());
                handled = true;
            }
        }
    }

    return handled;
}

void WidgetManager::setFocusedWindow(Window* window) {
    if (focused_window == window) return;

    if (focused_window) {
        focused_window->setFocus(false);
        markWidgetDirty(focused_window);
    }

    focused_window = window;

    if (focused_window) {
        focused_window->setFocus(true);
        markWidgetDirty(focused_window);
    }
}

void WidgetManager::setModalWindow(Window* window) {
    if (modal_window) {
        closeModalWindow(modal_window);
    }

    modal_window = window;
    if (modal_window) {
        modal_window->setModal(true);
        setFocusedWindow(modal_window);
    }
}

void WidgetManager::closeModalWindow(Window* window) {
    if (modal_window == window) {
        modal_window = nullptr;
        window->setModal(false);
        window->close();

        // Возвращаем фокус предыдущему окну
        if (!windows.empty()) {
            setFocusedWindow(windows.back().get());
        }
    }
}

void WidgetManager::updateBoundsCache() {
    widget_bounds_cache.clear();

    // Рекурсивное обновление для всех виджетов
    std::function<void(Widget*)> cacheWidget = [&](Widget* widget) {
        if (!widget) return;

        WidgetBounds bounds;
        bounds.widget = widget;
        bounds.x = widget->getX();
        bounds.y = widget->getY();
        bounds.width = widget->getWidth();
        bounds.height = widget->getHeight();
        bounds.visible = widget->isVisible();

        widget_bounds_cache.push_back(bounds);

        // Для контейнеров кэшируем дочерние элементы
        if (auto* container = dynamic_cast<Container*>(widget)) {
            for (auto& child : container->getChildren()) {
                cacheWidget(child.get());
            }
        }

        // Для окон кэшируем дочерние окна
        if (auto* window = dynamic_cast<Window*>(widget)) {
            for (auto* childWindow : window->getChildWindows()) {
                cacheWidget(childWindow);
            }
        }
    };

    for (auto& window : windows) {
        cacheWidget(window.get());
    }

    bounds_cache_valid = true;
}

Widget* WidgetManager::findWidgetAt(int x, int y) {
    if (!bounds_cache_valid) {
        updateBoundsCache();
    }

    // Поиск с конца (верхние элементы первыми)
    for (auto it = widget_bounds_cache.rbegin(); it != widget_bounds_cache.rend(); ++it) {
        const auto& bounds = *it;
        if (bounds.visible &&
            x >= bounds.x && x < bounds.x + bounds.width &&
            y >= bounds.y && y < bounds.y + bounds.height) {
            return bounds.widget;
        }
    }

    return nullptr;
}
