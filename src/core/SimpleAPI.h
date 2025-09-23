#ifndef SIMPLE_API_H
#define SIMPLE_API_H

#include "../window/Window.h"
#include "../window/Dialog.h"
#include "../widgets/Button.h"
#include "../widgets/Label.h"
#include "../widgets/TextBox.h"
#include "../widgets/CheckBox.h"
#include "../widgets/ProgressBar.h"
#include "../widgets/ListBox.h"
#include "../widgets/MenuBar.h"
#include "../input/Mouse.h"
#include "../core/Terminal.h"
#include <functional>
#include <string>
#include <memory>

// Упрощенный интерфейс для быстрого создания приложений
// ВСЕГДА использует оптимизации автоматически!

namespace ui {
    
    // Глобальный терминал (автоматически управляется)
    extern std::unique_ptr<Terminal> g_terminal;
    
    // Создание окон (автоматически используют оптимизации)
    Window* createWindow(int x, int y, int width, int height, const std::string& title = "");
    Window* createModalDialog(int x, int y, int width, int height, const std::string& title = "");
    
    // Создание виджетов
    Button* addButton(Window* parent, int x, int y, int width, const std::string& text, 
                     const std::function<void()>& onClick = nullptr);
    
    Label* addLabel(Window* parent, int x, int y, const std::string& text);
    
    TextBox* addTextBox(Window* parent, int x, int y, int width, const std::string& content = "");
    
    CheckBox* addCheckBox(Window* parent, int x, int y, const std::string& text, bool checked = false);
    
    ProgressBar* addProgressBar(Window* parent, int x, int y, int width, float progress = 0.0f);
    
    ListBox* addListBox(Window* parent, int x, int y, int width, int height);
    
    MenuBar* createMenuBar(Window* parent, int x, int y, int width);
    
    // Диалоги (автоматически модальные)
    void showMessage(const std::string& title, const std::string& message);
    bool showConfirm(const std::string& title, const std::string& message);
    std::string showInput(const std::string& title, const std::string& message, 
                         const std::string& defaultValue = "");
    
    // Стили
    void setWidgetStyle(Widget* widget, const ColorStyle& style);
    void setWidgetBoxStyle(Widget* widget, const BoxStyle& style);
    
    // События
    void setOnWindowClose(Window* window, const std::function<void()>& onClose);
    
    // Управление приложением (всегда оптимизировано!)
    void runApplication();  // Автоматически использует все оптимизации
    void exitApplication();
    
    // Получение текущего состояния
    Window* getFocusedWindow();
    MousePosition getMousePosition();
    
    // Утилиты
    void clearScreen();
    void refreshScreen();
    
    // Расширенное управление (для сложных приложений)
    void updateFrame();  // Один кадр обновления
    bool isApplicationRunning();
    void stopApplication();
    
}

#endif