#ifdef WINDOWS_PLATFORM

#include "Platform.h"
#include <windows.h>
#include <iostream>

static HANDLE hConsoleOutput = INVALID_HANDLE_VALUE;
static HANDLE hConsoleInput = INVALID_HANDLE_VALUE;
static DWORD originalOutputMode = 0;
static DWORD originalInputMode = 0;
static bool mouseEnabled = false;

bool Platform::enterRawMode() {
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    
    if (hConsoleOutput == INVALID_HANDLE_VALUE || hConsoleInput == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    // Сохраняем оригинальные режимы
    GetConsoleMode(hConsoleOutput, &originalOutputMode);
    GetConsoleMode(hConsoleInput, &originalInputMode);
    
    // Устанавливаем новые режимы
    DWORD newOutputMode = originalOutputMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    DWORD newInputMode = originalInputMode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT;
    
    SetConsoleMode(hConsoleOutput, newOutputMode);
    SetConsoleMode(hConsoleInput, newInputMode);
    
    return true;
}

void Platform::exitRawMode() {
    if (hConsoleOutput != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hConsoleOutput, originalOutputMode);
    }
    if (hConsoleInput != INVALID_HANDLE_VALUE) {
        SetConsoleMode(hConsoleInput, originalInputMode);
    }
}

void Platform::clearScreen() {
    system("cls");
}

void Platform::moveCursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsoleOutput, coord);
}

bool Platform::readKey(char& key) {
    DWORD eventsRead = 0;
    INPUT_RECORD buffer[128];
    
    if (PeekConsoleInput(hConsoleInput, buffer, 128, &eventsRead) && eventsRead > 0) {
        for (DWORD i = 0; i < eventsRead; i++) {
            if (buffer[i].EventType == KEY_EVENT) {
                KEY_EVENT_RECORD keyEvent = buffer[i].Event.KeyEvent;
                if (keyEvent.bKeyDown) {
                    if (keyEvent.uChar.AsciiChar != 0) {
                        key = keyEvent.uChar.AsciiChar;
                        ReadConsoleInput(hConsoleInput, &buffer[i], 1, &eventsRead);
                        return true;
                    }
                }
            }
        }
        // Очищаем обработанные события
        ReadConsoleInput(hConsoleInput, buffer, eventsRead, &eventsRead);
    }
    
    return false;
}

bool Platform::enableMouseInput() {
    if (hConsoleInput == INVALID_HANDLE_VALUE) return false;
    
    DWORD currentMode;
    GetConsoleMode(hConsoleInput, &currentMode);
    DWORD newMode = currentMode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
    mouseEnabled = SetConsoleMode(hConsoleInput, newMode);
    return mouseEnabled;
}

void Platform::disableMouseInput() {
    if (hConsoleInput != INVALID_HANDLE_VALUE && mouseEnabled) {
        DWORD currentMode;
        GetConsoleMode(hConsoleInput, &currentMode);
        DWORD newMode = currentMode & ~ENABLE_MOUSE_INPUT;
        SetConsoleMode(hConsoleInput, newMode);
        mouseEnabled = false;
    }
}

bool Platform::getMouseClick(int& x, int& y, MouseButton& button) {
    DWORD eventsRead = 0;
    INPUT_RECORD buffer[128];
    
    if (PeekConsoleInput(hConsoleInput, buffer, 128, &eventsRead) && eventsRead > 0) {
        for (DWORD i = 0; i < eventsRead; i++) {
            if (buffer[i].EventType == MOUSE_EVENT) {
                MOUSE_EVENT_RECORD mouseEvent = buffer[i].Event.MouseEvent;
                
                if (mouseEvent.dwEventFlags == 0) { // Клик/отпускание
                    if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                        x = mouseEvent.dwMousePosition.X;
                        y = mouseEvent.dwMousePosition.Y;
                        button = MouseButton::LEFT;
                        ReadConsoleInput(hConsoleInput, &buffer[i], 1, &eventsRead);
                        return true;
                    }
                    else if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                        x = mouseEvent.dwMousePosition.X;
                        y = mouseEvent.dwMousePosition.Y;
                        button = MouseButton::RIGHT;
                        ReadConsoleInput(hConsoleInput, &buffer[i], 1, &eventsRead);
                        return true;
                    }
                    else if (mouseEvent.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) {
                        x = mouseEvent.dwMousePosition.X;
                        y = mouseEvent.dwMousePosition.Y;
                        button = MouseButton::MIDDLE;
                        ReadConsoleInput(hConsoleInput, &buffer[i], 1, &eventsRead);
                        return true;
                    }
                }
            }
        }
        // Очищаем обработанные события
        ReadConsoleInput(hConsoleInput, buffer, eventsRead, &eventsRead);
    }
    
    return false;
}

bool Platform::getMouseRelease(int& x, int& y, MouseButton& button) {
    DWORD eventsRead = 0;
    INPUT_RECORD buffer[128];
    
    if (PeekConsoleInput(hConsoleInput, buffer, 128, &eventsRead) && eventsRead > 0) {
        for (DWORD i = 0; i < eventsRead; i++) {
            if (buffer[i].EventType == MOUSE_EVENT) {
                MOUSE_EVENT_RECORD mouseEvent = buffer[i].Event.MouseEvent;
                
                if (mouseEvent.dwEventFlags == 0) { // Отпускание кнопки
                    // Проверяем отпускание кнопок (когда состояние становится 0)
                    static DWORD lastButtonState = 0;
                    if (lastButtonState != 0 && mouseEvent.dwButtonState == 0) {
                        x = mouseEvent.dwMousePosition.X;
                        y = mouseEvent.dwMousePosition.Y;
                        // Определяем, какая кнопка была отпущена
                        if (lastButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                            button = MouseButton::LEFT;
                        } else if (lastButtonState & RIGHTMOST_BUTTON_PRESSED) {
                            button = MouseButton::RIGHT;
                        } else if (lastButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) {
                            button = MouseButton::MIDDLE;
                        }
                        lastButtonState = mouseEvent.dwButtonState;
                        ReadConsoleInput(hConsoleInput, &buffer[i], 1, &eventsRead);
                        return true;
                    }
                    lastButtonState = mouseEvent.dwButtonState;
                }
            }
        }
        // Очищаем обработанные события
        ReadConsoleInput(hConsoleInput, buffer, eventsRead, &eventsRead);
    }
    
    return false;
}

bool Platform::getMouseMove(int& x, int& y) {
    DWORD eventsRead = 0;
    INPUT_RECORD buffer[128];
    
    if (PeekConsoleInput(hConsoleInput, buffer, 128, &eventsRead) && eventsRead > 0) {
        for (DWORD i = 0; i < eventsRead; i++) {
            if (buffer[i].EventType == MOUSE_EVENT) {
                MOUSE_EVENT_RECORD mouseEvent = buffer[i].Event.MouseEvent;
                
                if (mouseEvent.dwEventFlags & MOUSE_MOVED) {
                    x = mouseEvent.dwMousePosition.X;
                    y = mouseEvent.dwMousePosition.Y;
                    ReadConsoleInput(hConsoleInput, &buffer[i], 1, &eventsRead);
                    return true;
                }
            }
        }
        // Очищаем обработанные события
        ReadConsoleInput(hConsoleInput, buffer, eventsRead, &eventsRead);
    }
    
    return false;
}

#endif