#ifndef WINDOW_H
#define WINDOW_H

#include "../app.h"

#include <memory> 

namespace GLTest {
    struct Window {
        GLFWwindow* nativeWindow;
        GLFWmonitor* monitor;
        int width;
        int height;
        bool isFullscreen;

        i32 cached_x, cached_y, cached_w, cached_h;

        void installMainCallbacks();

        void close();

        void swapBuffers();

        static Window* createWindow(int width, int height, const char* title, bool fullscreen=false);
        static void freeWindow(Window* window);

        void fullscreenSwitch();
    };
}

#endif
