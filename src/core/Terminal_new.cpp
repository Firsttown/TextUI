#include "Terminal.h"
#include "../platform/Platform.h"
#include "../input/Mouse.h"
#include <iostream>

Terminal::Terminal() : raw_mode_enabled(false), screen_width(80), screen_height(25), running(false) {
    render_buffer = std::make_unique<RenderBuffer>(screen_width, screen_height);
}

Terminal::~Terminal() {
    cleanup();
}

bool Terminal::initialize() {
    if (!Platform::enterRawMode()) {
        return false;
    }
    raw_mode_enabled = true;

    // Попытка включить мышь
    Mouse::enableMouse();

    return true;
}

void Terminal::cleanup() {
    if (raw_mode_enabled) {
        Platform::exitRawMode();
        raw_mode_enabled = false;
    }
    Mouse::disableMouse();
}

void Terminal::clear() {
    if (render_buffer) {
        render_buffer->clear();
    }
}

void Terminal::flush() {
    if (render_buffer && render_buffer->isDirty()) {
        render_buffer->renderToTerminal();
    }
}

void Terminal::resize(int new_width, int new_height) {
    if (new_width != screen_width || new_height != screen_height) {
        screen_width = new_width;
        screen_height = new_height;
        if (render_buffer) {
            render_buffer->resize(new_width, new_height);
        }
        widget_manager.invalidateBoundsCache();
    }
}

void Terminal::update() {
    // Оптимизированный рендеринг
    widget_manager.renderOptimized(*render_buffer);
    flush();

    // Обработка ввода
    char key;
    if (Platform::readKey(key)) {
        if (key == 'q' || key == 'Q') {
            running = false;
            return;
        }
        widget_manager.handleInputOptimized(key);
    }

    // Обработка мыши
    if (Mouse::isMouseEnabled()) {
        auto pos = Mouse::getCurrentPosition();

        static bool leftPressed = false;
        if (pos.leftPressed && !leftPressed) {
            widget_manager.handleMouseOptimized(pos.x, pos.y, MouseButton::LEFT, true);
            leftPressed = true;
        } else if (!pos.leftPressed && leftPressed) {
            widget_manager.handleMouseOptimized(pos.x, pos.y, MouseButton::LEFT, false);
            leftPressed = false;
        }

        static int lastX = -1, lastY = -1;
        if (lastX != pos.x || lastY != pos.y) {
            widget_manager.handleMouseMoveOptimized(pos.x, pos.y);
            lastX = pos.x;
            lastY = pos.y;
        }
    }
}

void Terminal::run() {
    running = true;

    while (running) {
        update();
    }
}
