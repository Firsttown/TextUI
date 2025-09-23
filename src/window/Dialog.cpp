#include "Dialog.h"
#include "src/core/Renderer.h"
#include "src/widgets/Label.h"
#include "src/widgets/Button.h"
#include <iostream>

Dialog::Dialog(int x, int y, int width, int height, const std::string& title)
    : Window(x, y, width, height, title) {
    modal = true;
    setBoxStyle(BoxStyle::doubleLine());
    setColorStyle(ColorStyle(Color::BRIGHT_WHITE, BackgroundColor::DEFAULT).setBold());
}

void Dialog::accept() {
    if (onAccept) {
        onAccept();
    }
    close();
}

void Dialog::cancel() {
    if (onCancel) {
        onCancel();
    }
    close();
}

// MessageBox implementation
MessageBox::MessageBox(const std::string& title, const std::string& msg)
    : ModalDialog(20, 10, 40, 10, title), message(msg) {

    // Добавляем кнопку OK
    okButton = addChild<Button>(getWidth() / 2 - 4, getHeight() - 3, 8, "OK");
    okButton->setOnClick([this]() {
        if (onOk) {
            onOk();
        }
        accept(); // Используем accept из базового класса
    });
    okButton->setFocus(true);
}

void MessageBox::render() {
    Window::render();

    // Рисуем сообщение
    if (!message.empty()) {
        int textX = getX() + (getWidth() - message.length()) / 2;
        int textY = getY() + 2;
        if (textX > getX() && textX + static_cast<int>(message.length()) < getX() + getWidth()) {
            Renderer::drawText(textX, textY, message, ColorStyle(Color::WHITE));
        }
    }
}

void MessageBox::accept() {
    if (onOk) {
        onOk();
    }
    completeWithResult();
}

// ConfirmDialog implementation
ConfirmDialog::ConfirmDialog(const std::string& title, const std::string& msg)
    : ModalDialog(15, 8, 50, 12, title), message(msg), dialogResult(false) {

    // Добавляем кнопки
    okButton = addChild<Button>(getWidth() / 2 - 12, getHeight() - 3, 10, "Yes");
    cancelButton = addChild<Button>(getWidth() / 2 + 2, getHeight() - 3, 10, "No");

    okButton->setOnClick([this]() {
        setAndComplete(true);
    });

    cancelButton->setOnClick([this]() {
        setAndComplete(false);
    });

    okButton->setFocus(true);
}

void ConfirmDialog::render() {
    Window::render();

    // Рисуем сообщение
    if (!message.empty()) {
        int textX = getX() + (getWidth() - message.length()) / 2;
        int textY = getY() + 2;
        if (textX > getX() && textX + static_cast<int>(message.length()) < getX() + getWidth()) {
            Renderer::drawText(textX, textY, message, ColorStyle(Color::WHITE));
        }
    }
}

bool ConfirmDialog::handleInput(char key) {
    if (key == '\n' || key == '\r') {
        // Enter - подтверждаем
        setAndComplete(true);
        return true;
    } else if (key == 27) { // Escape
        setAndComplete(false);
        return true;
    }

    return Window::handleInput(key);
}

void ConfirmDialog::setAndComplete(bool result) {
    dialogResult = result;
    if (onResult) {
        onResult(result);
    }
    completeWithResult();
}

// InputDialog implementation
InputDialog::InputDialog(const std::string& title, const std::string& msg, const std::string& defaultValue)
    : ModalDialog(15, 8, 50, 14, title), message(msg), dialogResult(defaultValue) {

    // Добавляем текстбокс
    textBox = addChild<TextBox>(4, 4, getWidth() - 8);
    textBox->setContent(defaultValue);
    textBox->setFocus(true);

    // Добавляем кнопки
    okButton = addChild<Button>(getWidth() / 2 - 12, getHeight() - 3, 10, "OK");
    cancelButton = addChild<Button>(getWidth() / 2 + 2, getHeight() - 3, 10, "Cancel");

    okButton->setOnClick([this]() {
        setAndComplete(textBox->getContent());
    });

    cancelButton->setOnClick([this]() {
        setAndComplete("");
    });
}

void InputDialog::render() {
    Window::render();

    // Рисуем сообщение
    if (!message.empty()) {
        int textX = getX() + (getWidth() - message.length()) / 2;
        int textY = getY() + 2;
        if (textX > getX() && textX + static_cast<int>(message.length()) < getX() + getWidth()) {
            Renderer::drawText(textX, textY, message, ColorStyle(Color::WHITE));
        }
    }
}

bool InputDialog::handleInput(char key) {
    if (key == '\n' || key == '\r') {
        // Enter - подтверждаем ввод
        setAndComplete(textBox->getContent());
        return true;
    } else if (key == '\t') {
        // Tab - переключаем фокус между элементами
        if (textBox->hasFocus()) {
            textBox->setFocus(false);
            okButton->setFocus(true);
        } else if (okButton->hasFocus()) {
            okButton->setFocus(false);
            cancelButton->setFocus(true);
        } else {
            cancelButton->setFocus(false);
            textBox->setFocus(true);
        }
        return true;
    } else if (key == 27) { // Escape
        setAndComplete("");
        return true;
    }

    // Передаем ввод активному элементу
    return Window::handleInput(key);
}

const std::string& InputDialog::getInputValue() const {
    return textBox->getContent();
}

void InputDialog::setAndComplete(const std::string& result) {
    dialogResult = result;
    if (onInput) {
        onInput(result);
    }
    completeWithResult();
}

// Функции для показа модальных диалогов с корректным управлением памятью
void showMessageDialog(Window* parent, const std::string& title, const std::string& message) {
    auto dialog = std::make_unique<MessageBox>(title, message);
    if (parent) {
        // Центрируем относительно родительского окна
        int centerX = parent->getX() + parent->getWidth() / 2;
        int centerY = parent->getY() + parent->getHeight() / 2;
        dialog->setPosition(centerX - dialog->getWidth() / 2,
                           centerY - dialog->getHeight() / 2);
    }

    // Устанавливаем модальное окно
    if (auto* terminal = ui::g_terminal.get()) {
        terminal->getWidgetManager().setModalWindow(dialog.get());
    }

    // Ожидаем завершения диалога
    dialog->waitForResult();
}

bool showConfirmDialog(Window* parent, const std::string& title, const std::string& message) {
    auto dialog = std::make_unique<ConfirmDialog>(title, message);
    if (parent) {
        // Центрируем относительно родительского окна
        int centerX = parent->getX() + parent->getWidth() / 2;
        int centerY = parent->getY() + parent->getHeight() / 2;
        dialog->setPosition(centerX - dialog->getWidth() / 2,
                           centerY - dialog->getHeight() / 2);
    }

    // Устанавливаем модальное окно
    if (auto* terminal = ui::g_terminal.get()) {
        terminal->getWidgetManager().setModalWindow(dialog.get());
    }

    // Ожидаем завершения диалога
    dialog->waitForResult();

    // Возвращаем результат
    return dialog->getResult();
}

std::string showInputDialog(Window* parent, const std::string& title, const std::string& message,
                           const std::string& defaultValue) {
    auto dialog = std::make_unique<InputDialog>(title, message, defaultValue);
    if (parent) {
        // Центрируем относительно родительского окна
        int centerX = parent->getX() + parent->getWidth() / 2;
        int centerY = parent->getY() + parent->getHeight() / 2;
        dialog->setPosition(centerX - dialog->getWidth() / 2,
                           centerY - dialog->getHeight() / 2);
    }

    // Устанавливаем модальное окно
    if (auto* terminal = ui::g_terminal.get()) {
        terminal->getWidgetManager().setModalWindow(dialog.get());
    }

    // Ожидаем завершения диалога
    dialog->waitForResult();

    // Возвращаем результат
    return dialog->getResult();
}
