#ifndef PLATFORM_H
#define PLATFORM_H

#include "../input/Mouse.h"

class Platform {
public:
    static bool enterRawMode();
    static void exitRawMode();
    static void clearScreen();
    static void moveCursor(int x, int y);
    static bool readKey(char& key);
    
    // Методы для работы с мышью
    static bool enableMouseInput();
    static void disableMouseInput();
    static bool getMouseClick(int& x, int& y, MouseButton& button);
    static bool getMouseRelease(int& x, int& y, MouseButton& button);
    static bool getMouseMove(int& x, int& y);
};

#endif