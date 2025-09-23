#include "../src/core/SimpleAPI.h"
#include "src/platform/Platform.h"
#include <iostream>
#include <string>
#include <thread>

using namespace ui;

// Функция для демонстрации работы с окнами и виджетами
void demonstrateBasicWidgets() {
    // Создаем главное окно
    auto* mainWindow = createWindow(5, 5, 70, 20, "Пример TextUI - Основные виджеты");

    // Добавляем метку
    addLabel(mainWindow, 5, 2, "Это пример использования фреймворка TextUI");

    // Добавляем текстовое поле
    auto* nameField = addTextBox(mainWindow, 5, 4, 30, "Введите ваше имя");

    // Добавляем чекбокс
    auto* showDetails = addCheckBox(mainWindow, 5, 6, "Показать детали", false);

    // Добавляем прогресс-бар
    auto* progressBar = addProgressBar(mainWindow, 5, 8, 30, 0.0f);

    // Добавляем кнопку
    addButton(mainWindow, 5, 10, 15, "Показать приветствие", [&]() {
        std::string name = nameField->getContent();
        if (name.empty()) {
            showMessage("Ошибка", "Пожалуйста, введите ваше имя");
        } else {
            std::string message = "Привет, " + name + "!";
            if (showDetails->isChecked()) {
                message += " Это демонстрация возможностей фреймворка TextUI. " "Фреймворк поддерживает:" "- Модальные окна" "- Различные виджеты" "- Цветовое оформление" "- Обработку событий";
            }
            showMessage("Приветствие", message);
        }
    });

    // Добавляем кнопку для демонстрации прогресс-бара
    addButton(mainWindow, 25, 10, 15, "Тест прогресса", [&]() {
        float progress = 0.0f;
        while (progress < 1.0f) {
            progress += 0.05f;
            progressBar->setProgress(progress);
            refreshScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Задержка в 100 мс
        }
        showMessage("Завершено", "Прогресс завершен!");
    });

    // Добавляем кнопку для демонстрации диалога подтверждения
    addButton(mainWindow, 45, 10, 20, "Проверить подтверждение", [&]() {
        bool confirmed = showConfirm("Подтверждение", "Вы уверены, что хотите продолжить?");
        showMessage("Результат", confirmed ? "Вы подтвердили действие" : "Вы отменили действие");
    });

    // Добавляем кнопку для демонстрации диалога ввода
    addButton(mainWindow, 5, 13, 25, "Запросить комментарий", [&]() {
        std::string comment = showInput("Комментарий", "Введите ваш комментарий:", "Пример комментария");
        showMessage("Ваш комментарий", comment);
    });

    // Добавляем кнопку для выхода
    addButton(mainWindow, 35, 13, 25, "Выход", [&]() {
        if (showConfirm("Выход", "Вы действительно хотите выйти?")) {
            exitApplication();
        }
    });
}

// Функция для демонстрации работы с модальными окнами
void demonstrateModalWindows() {
    // Создаем главное окно
    auto* modalWindow = createWindow(10, 5, 60, 15, "Демонстрация модальных окон");

    // Добавляем описание
    addLabel(modalWindow, 5, 2, "Эта демонстрация показывает работу модальных окон");

    // Кнопка для показа информационного окна
    addButton(modalWindow, 5, 5, 25, "Показать сообщение", [&]() {
        showMessage("Информационное окно", "Это простое информационное окно");
    });

    // Кнопка для показа окна подтверждения
    addButton(modalWindow, 35, 5, 25, "Показать подтверждение", [&]() {
        bool result = showConfirm("Подтверждение", "Вы согласны с условиями?");
        showMessage("Результат", result ? "Согласен" : "Не согласен");
    });

    // Кнопка для показа окна ввода
    addButton(modalWindow, 5, 8, 25, "Показать диалог ввода", [&]() {
        std::string input = showInput("Ввод данных", "Введите что-нибудь:", "По умолчанию");
        showMessage("Введенные данные", input);
    });

    // Кнопка для показа вложенных модальных окон
    addButton(modalWindow, 35, 8, 25, "Вложенные модальные окна", [&]() {
        // Показываем первое модальное окно
        bool firstResult = showConfirm("Первый диалог", "Это первый модальный диалог");

        // В зависимости от результата, показываем второе
        if (firstResult) {
            bool secondResult = showConfirm("Второй диалог", "Это второй модальный диалог");
            showMessage("Итог", "Результат: " + std::to_string(firstResult) + ", " + std::to_string(secondResult));
        }
    });

    // Кнопка для выхода
    addButton(modalWindow, 20, 12, 20, "Главное меню", [&]() {
        exitApplication(); // Завершаем текущее приложение
    });
}

int main() {
    // Инициализируем приложение
    runApplication();

    // Основное меню
    while (true) {
        // Создаем меню
        auto* menuWindow = createWindow(15, 5, 50, 15, "Главное меню");

        // Добавляем заголовок
        addLabel(menuWindow, 5, 2, "Выберите демонстрацию:");

        // Кнопки для выбора демонстрации
        addButton(menuWindow, 5, 5, 40, "1. Основные виджеты", [&]() {
            demonstrateBasicWidgets();
        });

        addButton(menuWindow, 5, 8, 40, "2. Модальные окна", [&]() {
            demonstrateModalWindows();
        });

        // Кнопка для выхода
        addButton(menuWindow, 5, 11, 40, "Выход", [&]() {
            exitApplication();
        });

        // Запускаем приложение
        while (isApplicationRunning()) {
            updateFrame();
        }

        // Если приложение было завершено, но не через выход, показываем меню снова
        if (!isApplicationRunning()) {
            runApplication();
        }
    }

    return 0;
}
