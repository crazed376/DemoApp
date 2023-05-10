#include "window.h"
#include "input.h"

namespace GLTest {
    static void resizeCallback(GLFWwindow* nativeWindow, int newWidth, int newHeight) {
        Window* window = (Window*)glfwGetWindowUserPointer(nativeWindow);
        window->width = newWidth;
        window->height = newHeight;
        glViewport(0, 0, newWidth, newHeight);
    }
    void Window::installMainCallbacks() {
        if(nativeWindow == nullptr) return;

        glfwSetKeyCallback(nativeWindow, Input::keyCallback);
        glfwSetCursorPosCallback(nativeWindow, Input::mouseCallback);
        glfwSetMouseButtonCallback(nativeWindow, Input::mouseButtonCallback);
        glfwSetWindowSizeCallback(nativeWindow, resizeCallback);
    }

    void Window::close() {
        if(nativeWindow == nullptr) return;
        glfwSetWindowShouldClose(nativeWindow, GLFW_TRUE);
    }

    Window* Window::createWindow(int width, int height, const char* title, bool fullscreen) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Only supply the monitor if we want to start the window in full-screen mode
        Window* res = new Window();
        GLFWmonitor* primaryMonitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        res->nativeWindow = glfwCreateWindow(width, height, title, primaryMonitor, nullptr);
        if(res->nativeWindow == nullptr) {
            printf("Failed to create GLFW window.\n");
            glfwTerminate();
            return nullptr;
        }
        res->monitor = glfwGetPrimaryMonitor();
        glfwMakeContextCurrent(res->nativeWindow);
        glfwSetWindowUserPointer(res->nativeWindow, (void*)res);

        res->width = width;
        res->height = height;

        return res;
    }

    void Window::freeWindow(Window* window) {
        if(window) {
            glfwDestroyWindow(window->nativeWindow);
            delete window;
        }
    }

    void Window::fullscreenSwitch() {
        if(isFullscreen) {
            glfwGetWindowPos(nativeWindow, &cached_x, &cached_y);
            glfwGetWindowSize(nativeWindow, &cached_w, &cached_h);

            // Get resolution of monitor
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            // Switch to full screen
            glfwSetWindowMonitor(nativeWindow, monitor, 0, 0, mode->width, mode->height, 0);
        } else {
            glfwSetWindowMonitor(nativeWindow, nullptr, cached_x, cached_y, cached_w, cached_h, 0);
        }

        isFullscreen = !isFullscreen;
    }
}
