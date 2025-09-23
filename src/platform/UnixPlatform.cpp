#ifdef UNIX_PLATFORM

#include "Platform.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <iostream>
#include <poll.h>

static struct termios original_termios;
static bool raw_mode_enabled = false;
static bool mouse_enabled = false;

bool Platform::enterRawMode() {
    if (!isatty(STDIN_FILENO)) return false;
    
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) return false;
    
    struct termios raw = original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return false;
    
    raw_mode_enabled = true;
    return true;
}

void Platform::exitRawMode() {
    if (raw_mode_enabled && tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) != -1) {
        raw_mode_enabled = false;
    }
    if (mouse_enabled) {
        disableMouseInput();
    }
}

void Platform::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void Platform::moveCursor(int x, int y) {
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H" << std::flush;
}

bool Platform::readKey(char& key) {
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    int ret = poll(&pfd, 1, 0);
    
    if (ret > 0 && (pfd.revents & POLLIN)) {
        int bytes = read(STDIN_FILENO, &key, 1);
        return bytes == 1;
    }
    
    return false;
}

bool Platform::enableMouseInput() {
    // Включаем режим мыши в терминале
    std::cout << "\033[?1000h\033[?1002h\033[?1015h\033[?1006h" << std::flush;
    mouse_enabled = true;
    return true;
}

void Platform::disableMouseInput() {
    if (mouse_enabled) {
        std::cout << "\033[?1006l\033[?1015l\033[?1002l\033[?1000l" << std::flush;
        mouse_enabled = false;
    }
}

bool Platform::getMouseClick(int& x, int& y, MouseButton& button) {
    char buffer[32];
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    int ret = poll(&pfd, 1, 0);
    
    if (ret > 0 && (pfd.revents & POLLIN)) {
        int bytes = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            
            // Проверяем, является ли это событием мыши
            if (bytes >= 3 && buffer[0] == '\033' && buffer[1] == '[') {
                // Проверяем формат SGR (более современный)
                if (strncmp(&buffer[2], "<", 1) == 0) {
                    // Пример: \033[<0;10;20M - клик левой кнопки в позиции (10,20)
                    int button_code, mouse_x, mouse_y;
                    if (sscanf(&buffer[3], "%d;%d;%d", &button_code, &mouse_x, &mouse_y) == 3) {
                        x = mouse_x - 1; // Терминалы обычно используют 1-based координаты
                        y = mouse_y - 1;
                        
                        // Определяем тип кнопки
                        if (button_code == 0) { // Левая кнопка нажата
                            button = MouseButton::LEFT;
                            return true;
                        } else if (button_code == 1) { // Средняя кнопка нажата
                            button = MouseButton::MIDDLE;
                            return true;
                        } else if (button_code == 2) { // Правая кнопка нажата
                            button = MouseButton::RIGHT;
                            return true;
                        }
                    }
                }
                // Проверяем формат X10 (старый формат)
                else if (buffer[2] == 'M' && bytes >= 6) {
                    // Событие мыши в формате X10
                    unsigned char b = buffer[3] - 32;
                    x = buffer[4] - 33;
                    y = buffer[5] - 33;
                    
                    // Определяем тип события
                    if (b & 0x01) { // Левая кнопка
                        button = MouseButton::LEFT;
                        return true;
                    } else if (b & 0x02) { // Правая кнопка
                        button = MouseButton::RIGHT;
                        return true;
                    } else if (b & 0x04) { // Средняя кнопка
                        button = MouseButton::MIDDLE;
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

bool Platform::getMouseRelease(int& x, int& y, MouseButton& button) {
    // В Unix терминалах отпускание кнопки обычно кодируется особым образом
    char buffer[32];
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    int ret = poll(&pfd, 1, 0);
    
    if (ret > 0 && (pfd.revents & POLLIN)) {
        int bytes = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            
            if (bytes >= 3 && buffer[0] == '\033' && buffer[1] == '[') {
                if (strncmp(&buffer[2], "<", 1) == 0) {
                    int button_code, mouse_x, mouse_y;
                    if (sscanf(&buffer[3], "%d;%d;%d", &button_code, &mouse_x, &mouse_y) == 3) {
                        if (button_code >= 32 && button_code < 35) { // Отпускание кнопки
                            x = mouse_x - 1;
                            y = mouse_y - 1;
                            button = static_cast<MouseButton>(button_code - 32);
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

bool Platform::getMouseMove(int& x, int& y) {
    char buffer[32];
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    int ret = poll(&pfd, 1, 0);
    
    if (ret > 0 && (pfd.revents & POLLIN)) {
        int bytes = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            
            if (bytes >= 3 && buffer[0] == '\033' && buffer[1] == '[') {
                if (strncmp(&buffer[2], "<", 1) == 0) {
                    int button_code, mouse_x, mouse_y;
                    if (sscanf(&buffer[3], "%d;%d;%d", &button_code, &mouse_x, &mouse_y) == 3) {
                        // Движение мыши
                        if (button_code >= 35 && button_code < 64) { // Движение с нажатой кнопкой
                            x = mouse_x - 1;
                            y = mouse_y - 1;
                            return true;
                        } else if (button_code >= 64) { // Простое движение
                            x = mouse_x - 1;
                            y = mouse_y - 1;
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

#endif