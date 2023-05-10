#include "app.h"
#include "core/window.h"
#include "core/input.h"
#include "renderer/shaderProgram.h"
#include "challenges/buffers.h"
#include "challenges/shaderSquare.h"
#include "challenges/shaderExamples.h"
#include "challenges/terrain.h"
#include "challenges/cubes.h"

using namespace GLTest;

extern "C" {
    __declspec(dllexport) long long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

const int windowWidth = 1280;
const int windowHeight = 720;
const char* windowTitle = "OpenGL Tests";

void GLAPIENTRY errorMessageCallback(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar* message,
                                     const void* userParam) {
    if(type == GL_DEBUG_TYPE_ERROR) {
        g_logger_error("GL CALLBACK: **GL ERROR** type=0x%x, severity=0x%x, message=%s\n",  type, severity,  message);
    }
}

void glfwErrorCallback(int error, const char* description) {
    g_logger_error("GLFW CALLBACK: **GLFW ERROR** Error: %d, description: %s\n", error, description);
}

int main(void) {
    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()) {
        g_logger_error("GLFW failed to initialize.");
        return -1;
    }

    Window* window = Window::createWindow(windowWidth, windowHeight, windowTitle);
    if(window == nullptr) {
        g_logger_error("Failed to create GLFW window.");
        glfwTerminate();
        return -1;
    }
    window->installMainCallbacks();

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        g_logger_error("Failed to initialize GLAD.");
        glfwTerminate();
        return -1;
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(errorMessageCallback, 0);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, windowWidth, windowHeight);

    float dt = 0.016f;
    float frameStart = 0.0f;

    // Initialize challenges
    BufferChallenge::init(*window);
    ShaderExamples::init(*window);
    TerrainShader::init(*window);
    Cubes::init(*window);

    bool showBuffer = false;
    bool showExamples = false;
    bool showTerrain = false;
    bool showCubes = false;
    float keyDebounce = 0.1f;

    while(!glfwWindowShouldClose(window->nativeWindow)) {
        dt = glfwGetTime() - frameStart;
        frameStart = glfwGetTime();

        glClearColor(39.0f/255.0f, 40.0f/255.0f, 34.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        keyDebounce -= dt;
        
        if(Input::isKeyDown(GLFW_KEY_Q) && keyDebounce < 0.0f) {
            window->close();
            keyDebounce = 0.3f;
        }
        
        // Fullscreen
        if(Input::isKeyDown(GLFW_KEY_F) && keyDebounce < 0.0f) {
            window->fullscreenSwitch();
            keyDebounce = 0.3f;
        }

        // Change which challenge to show
        if(Input::isKeyDown(GLFW_KEY_B) && keyDebounce < 0.0f) {
            showBuffer = true;
            showExamples = false;
            showTerrain = false;
            showCubes = false;
            keyDebounce = 0.3f;
            g_logger_info("Showing Buffer challenges.");
        } else if(Input::isKeyDown(GLFW_KEY_P) && keyDebounce < 0.0f) {
            showBuffer = false;
            showExamples = true;
            showTerrain = false;
            showCubes = false;
            keyDebounce = 0.3f;
            g_logger_info("Showing Shader box challenges.");
        } else if(Input::isKeyDown(GLFW_KEY_T) && keyDebounce < 0.0f) {
            showBuffer = false;
            showExamples = false;
            showTerrain = true;
            showCubes = false;
            keyDebounce = 0.3f;
            g_logger_info("Showing Shader terrain challenges.");
        } else if(Input::isKeyDown(GLFW_KEY_C) && keyDebounce < 0.0f) {
            showBuffer = false;
            showExamples = false;
            showTerrain = false;
            showCubes = true;
            keyDebounce = 0.3f;
            g_logger_info("Showing Texture challenges.");
        }

        if(showBuffer) {
            glDisable(GL_CULL_FACE); 
            BufferChallenge::update(dt);
        } else if(showExamples) {
            glDisable(GL_CULL_FACE);
            ShaderExamples::update(dt);
        } else if(showTerrain) {
            glDisable(GL_CULL_FACE);
            TerrainShader::update(dt);
        } else if(showCubes) {
            glEnable(GL_CULL_FACE);
            Cubes::update(dt);
        }

        glfwSwapBuffers(window->nativeWindow);
        glfwPollEvents();
    }

    // Destroy challenges
    BufferChallenge::destroy();
    ShaderExamples::destroy();
    TerrainShader::destroy();
    Cubes::destroy();

    Window::freeWindow(window);
    glfwTerminate();
    return 0;
}