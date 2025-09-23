#ifndef TERMINAL_H
#define TERMINAL_H

#include "RenderBuffer.h"
#include "../core/WidgetManager.h"
#include <memory>

class Terminal {
private:
    bool raw_mode_enabled;
    std::unique_ptr<RenderBuffer> render_buffer;
    WidgetManager widget_manager;
    int screen_width;
    int screen_height;
    bool running;

public:
    Terminal();
    ~Terminal();
    
    bool initialize();
    void cleanup();
    
    void clear();
    void flush();
    
    void resize(int new_width, int new_height);
    
    WidgetManager& getWidgetManager() { return widget_manager; }
    
    // Автоматически оптимизированный главный цикл
    void run();
    
    // Методы для ручного управления (если нужно)
    void update(); // Один кадр обновления
    bool isRunning() const { return running; }
    void stop() { running = false; }
    
    // Методы для работы с буфером
    RenderBuffer& getRenderBuffer() { return *render_buffer; }
    
    int getWidth() const { return screen_width; }
    int getHeight() const { return screen_height; }
};

#endif