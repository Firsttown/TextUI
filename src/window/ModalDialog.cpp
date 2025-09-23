#include "Dialog.h"
#include "src/core/Renderer.h"
#include "src/widgets/Label.h"
#include "src/widgets/Button.h"
#include <iostream>

ModalDialog::ModalDialog(int x, int y, int width, int height, const std::string& title)
    : Dialog(x, y, width, height, title) {
    completed = false;
    resultReady = false;
}

void ModalDialog::waitForResult() {
    std::unique_lock<std::mutex> lock(resultMutex);
    resultCondition.wait(lock, [this] { return resultReady; });
}

void ModalDialog::completeWithResult() {
    {
        std::lock_guard<std::mutex> lock(resultMutex);
        resultReady = true;
        completed = true;
    }
    resultCondition.notify_one();
}
