# TextUI

Кроссплатформенный фреймворк для создания псевдографических интерфейсов в терминале на C++.

## 🌟 Особенности

- **Кроссплатформенность**: Windows и Unix/Linux
- **Полноценный UI**: окна, виджеты, диалоги, меню
- **Поддержка мыши**: клики, движение, перетаскивание
- **Цвета и стили**: 16 цветов, разные стили рамок
- **Модальные окна**: блокирующие диалоги

## 📦 Установка

### Требования
- C++17 компилятор (GCC 7+, Clang 5+, Visual Studio 2017+)
- CMake 3.10+

### Сборка

```bash
git clone https://github.com/yourusername/textui.git
cd textui
mkdir build
cd build
cmake ..
make
```

Для Windows:
```cmd
mkdir build
cd build
cmake ..
cmake --build .
```

## 🚀 Быстрый старт

### Простой пример

```cpp
#include "core/SimpleAPI.h"

using namespace ui;

int main() {
    // Создаем окно
    auto* window = createWindow(10, 5, 50, 15, "My App");
    
    // Добавляем виджеты
    addLabel(window, 15, 1, "Hello, TextUI!");
    
    addButton(window, 5, 3, 15, "Click Me", []() {
        showMessage("Info", "Button clicked!");
    });
    
    addCheckBox(window, 5, 5, "Enable feature", true);
    
    // Запускаем приложение
    runApplication();
    
    return 0;
}
```

## 🎨 Компоненты

### Окна
- **Window**: основные окна с поддержкой перетаскивания
- **Dialog**: диалоговые окна
- **Modal windows**: модальные окна с блокировкой ввода

### Виджеты
- **Button**: кнопки с обработчиками клика
- **Label**: текстовые метки
- **TextBox**: поля ввода текста
- **CheckBox**: чекбоксы
- **ProgressBar**: индикаторы прогресса
- **ListBox**: списки с прокруткой
- **MenuBar**: горизонтальные меню
- **Container**: контейнеры для компоновки

### Цвета и стили

```cpp
// Предопределенные стили
ColorStyle::error()     // Красный жирный текст
ColorStyle::warning()   // Желтый текст
ColorStyle::success()   // Зеленый текст
ColorStyle::info()      // Синий текст
ColorStyle::highlight() // Выделение

// Стили рамок
BoxStyle::simple()      // Простые линии (+-|)
BoxStyle::singleLine()  // Одинарные линии (┌┐└┘)
BoxStyle::doubleLine()  // Двойные линии (╔╗╚╝)
BoxStyle::rounded()     // Закругленные углы (╭╮╰╯)
```

## 🖱️ Поддержка мыши

Фреймворк поддерживает:
- Клики левой/правой кнопкой мыши
- Движение курсора
- Перетаскивание окон
- Hover эффекты для виджетов

## 📚 API Reference

### Создание окон

```cpp
Window* createWindow(int x, int y, int width, int height, const std::string& title = "");
Window* createModalDialog(int x, int y, int width, int height, const std::string& title = "");
```

### Создание виджетов

```cpp
Button* addButton(Window* parent, int x, int y, int width, const std::string& text, 
                 const std::function<void()>& onClick = nullptr);

Label* addLabel(Window* parent, int x, int y, const std::string& text);

TextBox* addTextBox(Window* parent, int x, int y, int width, const std::string& content = "");

CheckBox* addCheckBox(Window* parent, int x, int y, const std::string& text, bool checked = false);

ProgressBar* addProgressBar(Window* parent, int x, int y, int width, float progress = 0.0f);

ListBox* addListBox(Window* parent, int x, int y, int width, int height);
```

### Диалоги

```cpp
void showMessage(const std::string& title, const std::string& message);
bool showConfirm(const std::string& title, const std::string& message);
std::string showInput(const std::string& title, const std::string& message, 
                     const std::string& defaultValue = "");
```

### Управление приложением

```cpp
void runApplication();    // Запуск главного цикла
void exitApplication();   // Завершение приложения
```

## 🛠️ Расширенное использование

### Низкоуровневый API
Для сложных приложений можно использовать низкоуровневый API:

```cpp
#include "window/Window.h"
#include "widgets/Button.h"
#include "input/Mouse.h"

// Создание окна через конструктор
Window* window = new Window(10, 5, 50, 15, "Advanced App");

// Добавление виджетов через addChild
Button* button = window->addChild<Button>(5, 3, 15, "Click");

// Ручное управление событиями
window->handleInput(key);
window->handleMouse(mouseX, mouseY, MouseButton::LEFT, isPressed);
```

## 📖 Документация по компонентам

### Window

```cpp
Window* window = createWindow(10, 5, 50, 15, "Title");

// Свойства
window->setTitle("New Title");
window->setPosition(20, 10);
window->setSize(60, 20);
window->setVisible(false);

// Методы
window->bringToFront();  // На передний план
window->close();         // Закрыть окно
```

### Button

```cpp
Button* button = addButton(window, 5, 3, 15, "Click Me", []() {
    std::cout << "Button clicked!" << std::endl;
});

// Дополнительные методы
button->setFocus(true);
button->setEnabled(false);
```

## 🐛 Известные ограничения

1. **Unicode**: ограниченная поддержка Unicode символов
2. **Размер терминала**: не адаптируется к изменению размера терминала

## 📄 Лицензия

MIT License - смотрите файл [LICENSE](LICENSE) для подробностей.