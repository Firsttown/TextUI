#include "SimpleAPI.h"
#include "src/window/Dialog.h"
#include <memory>
#include <vector>

// Глобальный терминал
std::unique_ptr<Terminal> ui::g_terminal;

namespace ui {

    Window* createWindow(int x, int y, int width, int height, const std::string& title) {
        if (!g_terminal) {
            g_terminal = std::make_unique<Terminal>();
            g_terminal->initialize();
        }
        return g_terminal->getWidgetManager().addWindow<Window>(x, y, width, height, title);
    }

    Window* createModalDialog(int x, int y, int width, int height, const std::string& title) {
        if (!g_terminal) {
            g_terminal = std::make_unique<Terminal>();
            g_terminal->initialize();
        }
        auto* dialog = new Dialog(x, y, width, height, title);
        g_terminal->getWidgetManager().setModalWindow(dialog);
        return dialog;
    }

    Button* addButton(Window* parent, int x, int y, int width, const std::string& text,
                     const std::function<void()>& onClick) {
        if (!parent) return nullptr;
        auto* button = parent->addChild<Button>(x, y, width, text);
        if (onClick) {
            button->setOnClick(onClick);
        }
        return button;
    }

    Label* addLabel(Window* parent, int x, int y, const std::string& text) {
        if (!parent) return nullptr;
        return parent->addChild<Label>(x, y, text);
    }

    TextBox* addTextBox(Window* parent, int x, int y, int width, const std::string& content) {
        if (!parent) return nullptr;
        auto* textBox = parent->addChild<TextBox>(x, y, width);
        textBox->setContent(content);
        return textBox;
    }

    CheckBox* addCheckBox(Window* parent, int x, int y, const std::string& text, bool checked) {
        if (!parent) return nullptr;
        auto* checkBox = parent->addChild<CheckBox>(x, y, text);
        checkBox->setChecked(checked);
        return checkBox;
    }

    ProgressBar* addProgressBar(Window* parent, int x, int y, int width, float progress) {
        if (!parent) return nullptr;
        auto* progressBar = parent->addChild<ProgressBar>(x, y, width);
        progressBar->setProgress(progress);
        return progressBar;
    }

    ListBox* addListBox(Window* parent, int x, int y, int width, int height) {
        if (!parent) return nullptr;
        return parent->addChild<ListBox>(x, y, width, height);
    }

    MenuBar* createMenuBar(Window* parent, int x, int y, int width) {
        if (!parent) return nullptr;
        return parent->addChild<MenuBar>(x, y, width);
    }

    // Реализация showMessage с использованием MessageBox
    void showMessage(const std::string& title, const std::string& message) {
        if (!g_terminal) {
            g_terminal = std::make_unique<Terminal>();
            g_terminal->initialize();
        }

        // Используем новую функцию с корректным управлением памятью
        showMessageDialog(nullptr, title, message);
    }

    // Реализация showConfirm с использованием ConfirmDialog
    bool showConfirm(const std::string& title, const std::string& message) {
        if (!g_terminal) {
            g_terminal = std::make_unique<Terminal>();
            g_terminal->initialize();
        }

        // Используем новую функцию с корректным управлением памятью
        return showConfirmDialog(nullptr, title, message);
    }

    // Реализация showInput с использованием InputDialog
    std::string showInput(const std::string& title, const std::string& message,
                         const std::string& defaultValue) {
        if (!g_terminal) {
            g_terminal = std::make_unique<Terminal>();
            g_terminal->initialize();
        }

        // Используем новую функцию с корректным управлением памятью
        return showInputDialog(nullptr, title, message, defaultValue);
    }

    void setWidgetStyle(Widget* widget, const ColorStyle& style) {
        if (widget) {
            widget->setColorStyle(style);
        }
    }

    void setWidgetBoxStyle(Widget* widget, const BoxStyle& style) {
        if (widget) {
            widget->setBoxStyle(style);
        }
    }

    void setOnWindowClose(Window* window, const std::function<void()>& onClose) {
        if (window && onClose) {
            window->setOnClose(onClose);
        }
    }

    void runApplication() {
        if (!g_terminal) {
            g_terminal = std::make_unique<Terminal>();
            if (!g_terminal->initialize()) {
                return;
            }
        }

        // Автоматически используем все оптимизации!
        g_terminal->run(); // Это уже оптимизированный метод
    }

    void exitApplication() {
        if (g_terminal) {
            g_terminal->stop();
        }
    }

    Window* getFocusedWindow() {
        if (g_terminal) {
            return g_terminal->getWidgetManager().getFocusedWindow();
        }
        return nullptr;
    }

    MousePosition getMousePosition() {
        return Mouse::getCurrentPosition();
    }

    void clearScreen() {
        if (g_terminal) {
            g_terminal->clear();
        }
    }

    void refreshScreen() {
        if (g_terminal) {
            g_terminal->flush();
        }
    }

    void updateFrame() {
        if (g_terminal) {
            g_terminal->update();
        }
    }

    bool isApplicationRunning() {
        return g_terminal && g_terminal->isRunning();
    }

    void stopApplication() {
        if (g_terminal) {
            g_terminal->stop();
        }
    }

}
