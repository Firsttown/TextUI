#ifndef MODAL_WINDOW_MANAGER_H
#define MODAL_WINDOW_MANAGER_H

#include "Window.h"
#include <vector>
#include <memory>
#include <algorithm>

class ModalWindowManager {
private:
    std::vector<std::unique_ptr<Window>> windows;
    Window* focusedWindow;
    Window* modalWindow;
    std::vector<Window*> windowStack; // Для отслеживания порядка окон

public:
    ModalWindowManager() : focusedWindow(nullptr), modalWindow(nullptr) {}
    
    template<typename T, typename... Args>
    T* addWindow(Args&&... args) {
        auto window = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = window.get();
        windows.push_back(std::move(window));
        windowStack.push_back(ptr);
        
        if (!focusedWindow) {
            focusedWindow = ptr;
            focusedWindow->setFocus(true);
        }
        return ptr;
    }
    
    Window* showModalWindow(Window* window) {
        if (modalWindow) {
            // Уже есть модальное окно
            return modalWindow;
        }
        
        modalWindow = window;
        modalWindow->setModal(true);
        modalWindow->bringToFront();
        
        // Устанавливаем фокус на модальное окно
        if (focusedWindow) {
            focusedWindow->setFocus(false);
        }
        focusedWindow = modalWindow;
        focusedWindow->setFocus(true);
        
        return modalWindow;
    }
    
    void closeModalWindow(Window* window) {
        if (modalWindow == window) {
            modalWindow = nullptr;
            window->close();
            
            // Возвращаем фокус предыдущему окну
            if (!windowStack.empty()) {
                for (auto it = windowStack.rbegin(); it != windowStack.rend(); ++it) {
                    if (*it != window && (*it)->isVisible()) {
                        focusedWindow = *it;
                        focusedWindow->setFocus(true);
                        break;
                    }
                }
            }
        }
    }
    
    void render() {
        // Рисуем все окна
        for (auto& window : windows) {
            if (window->isVisible()) {
                window->render();
            }
        }
        
        // Если есть модальное окно, рисуем оверлей
        if (modalWindow && modalWindow->isVisible()) {
            renderModalOverlay();
        }
    }
    
    bool handleInput(char key) {
        // Если есть модальное окно, передаем ввод только ему
        if (modalWindow && modalWindow->isVisible()) {
            return modalWindow->handleInput(key);
        }
        
        // Иначе передаем активному окну
        if (focusedWindow && focusedWindow->isVisible()) {
            return focusedWindow->handleInput(key);
        }
        
        return false;
    }
    
    bool handleMouse(int x, int y, MouseButton button, bool isPress) {
        // Если есть модальное окно, обрабатываем только его
        if (modalWindow && modalWindow->isVisible()) {
            if (modalWindow->isPointInside(x, y)) {
                return modalWindow->handleMouse(x, y, button, isPress);
            } else if (button == MouseButton::LEFT && isPress) {
                // Клик вне модального окна - игнорируем
                return true;
            }
            return false;
        }
        
        // Проверяем окна в обратном порядке (верхние первыми)
        for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
            if ((*it)->isVisible() && (*it)->isPointInside(x, y)) {
                // Устанавливаем фокус
                if (focusedWindow != it->get()) {
                    if (focusedWindow) {
                        focusedWindow->setFocus(false);
                    }
                    focusedWindow = it->get();
                    focusedWindow->setFocus(true);
                }
                
                if ((*it)->handleMouse(x, y, button, isPress)) {
                    if (button == MouseButton::LEFT && isPress) {
                        // Перемещаем окно на передний план
                        (*it)->bringToFront();
                        // Обновляем порядок в стеке
                        auto windowIt = std::find(windowStack.begin(), windowStack.end(), it->get());
                        if (windowIt != windowStack.end()) {
                            windowStack.erase(windowIt);
                            windowStack.push_back(it->get());
                        }
                    }
                    return true;
                }
            }
        }
        
        // Если клик вне окон, снимаем фокус
        if (button == MouseButton::LEFT && isPress) {
            if (focusedWindow) {
                focusedWindow->setFocus(false);
                focusedWindow = nullptr;
            }
        }
        
        return false;
    }
    
    void handleMouseMove(int x, int y) {
        // Если есть модальное окно, передаем движение только ему
        if (modalWindow && modalWindow->isVisible()) {
            modalWindow->handleMouseMove(x, y);
            return;
        }
        
        // Передаем движение активному окну
        if (focusedWindow && focusedWindow->isVisible()) {
            focusedWindow->handleMouseMove(x, y);
        }
        
        // Также передаем всем окнам для обработки hover эффектов
        for (auto& window : windows) {
            if (window->isVisible()) {
                window->handleMouseMove(x, y);
            }
        }
    }
    
    Window* getFocusedWindow() const { return focusedWindow; }
    Window* getModalWindow() const { return modalWindow; }
    
    void removeWindow(Window* window) {
        // Удаляем из вектора
        auto it = std::find_if(windows.begin(), windows.end(),
            [window](const std::unique_ptr<Window>& w) { return w.get() == window; });
        if (it != windows.end()) {
            windows.erase(it);
        }
        
        // Удаляем из стека
        auto stackIt = std::find(windowStack.begin(), windowStack.end(), window);
        if (stackIt != windowStack.end()) {
            windowStack.erase(stackIt);
        }
        
        // Обновляем фокус, если удаляемое окно было активным
        if (focusedWindow == window) {
            focusedWindow = nullptr;
            if (!windowStack.empty()) {
                for (auto it = windowStack.rbegin(); it != windowStack.rend(); ++it) {
                    if ((*it)->isVisible()) {
                        focusedWindow = *it;
                        focusedWindow->setFocus(true);
                        break;
                    }
                }
            }
        }
        
        // Обновляем модальное окно, если удаляемое было модальным
        if (modalWindow == window) {
            modalWindow = nullptr;
        }
    }

private:
    void renderModalOverlay() {
        // Рисуем полупрозрачный оверлей под модальным окном
        // В текстовом режиме можно использовать затемнение или рамку
    }
};

#endif