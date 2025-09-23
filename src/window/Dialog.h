#ifndef DIALOG_H
#define DIALOG_H

#include "Window.h"
#include "src/widgets/Button.h"
#include "src/widgets/Label.h"
#include "src/widgets/TextBox.h"
#include "src/core/WidgetManager.h"
#include <functional>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Базовый класс для модальных диалогов с поддержкой синхронизации
class ModalDialog : public Dialog {
protected:
    std::atomic<bool> completed;
    std::mutex resultMutex;
    std::condition_variable resultCondition;
    bool resultReady;

public:
    ModalDialog(int x, int y, int width, int height, const std::string& title);
    
    // Блокирующая функция ожидания результата
    virtual void waitForResult();
    
    // Функция для завершения диалога с результатом
    void completeWithResult();
    
    // Проверка, завершен ли диалог
    bool isCompleted() const { return completed; }
};

class Dialog : public Window {
protected:
    std::function<void()> onAccept;
    std::function<void()> onCancel;

public:
    Dialog(int x, int y, int width, int height, const std::string& title);
    
    void setOnAccept(const std::function<void()>& callback) { onAccept = callback; }
    void setOnCancel(const std::function<void()>& callback) { onCancel = callback; }
    
    void accept();
    void cancel();
};

class MessageBox : public ModalDialog {
private:
    std::string message;
    std::function<void()> onOk;
    Button* okButton;

public:
    MessageBox(const std::string& title, const std::string& msg);
    
    void setOnOk(const std::function<void()>& callback) { onOk = callback; }
    
    void render() override;
};

class ConfirmDialog : public Dialog {
private:
    std::string message;
    std::function<void(bool)> onResult;
    Button* okButton;
    Button* cancelButton;

public:
    ConfirmDialog(const std::string& title, const std::string& msg);
    
    void setOnResult(const std::function<void(bool)>& callback) { onResult = callback; }
    
    void render() override;
    bool handleInput(char key) override;
};

class InputDialog : public Dialog {
private:
    std::string message;
    TextBox* textBox;
    std::function<void(const std::string&)> onInput;
    Button* okButton;
    Button* cancelButton;

public:
    InputDialog(const std::string& title, const std::string& msg, const std::string& defaultValue = "");
    
    void setOnInput(const std::function<void(const std::string&)>& callback) { onInput = callback; }
    
    void render() override;
    bool handleInput(char key) override;
    const std::string& getInputValue() const;
};

// Функции для показа модальных диалогов (для внутреннего использования)
void showMessageDialog(Window* parent, const std::string& title, const std::string& message);
bool showConfirmDialog(Window* parent, const std::string& title, const std::string& message);
std::string showInputDialog(Window* parent, const std::string& title, const std::string& message, 
                           const std::string& defaultValue = "");

#endif