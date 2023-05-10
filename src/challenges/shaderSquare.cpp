#include "shaderSquare.h"
#include "../app.h"
#include "../core/input.h"
#include "../core/window.h"
#include "../renderer/shaderProgram.h"

namespace GLTest {
    namespace ShaderSquare {
        struct Vertex {
            glm::vec3 position;
        };

        static std::array<Vertex, 4> vertices = {
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f)}, // Bottom left
            Vertex{glm::vec3(-0.5f,  0.5f, 0.0f)}, // Top left
            Vertex{glm::vec3( 0.5f,  0.5f, 0.0f)}, // Top Right
            Vertex{glm::vec3( 0.5f, -0.5f, 0.0f)}, // Bottom Right
        };

        static std::array<u32, 6> indices = {0, 1, 2, 0, 2, 3};

        static u32 vao;
        static u32 vbo;
        static u32 ebo;

        static ShaderProgram basicShader;

        void initSquare() {
            glCreateVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Set up and buffer element buffer
            glCreateBuffers(1, &ebo);
            glNamedBufferData(ebo, sizeof(indices), indices.data(), GL_STATIC_DRAW);
            glVertexArrayElementBuffer(vao, ebo);

            // Set up and buffer the vertex buffer
            glCreateBuffers(1, &vbo);
            glNamedBufferData(vbo, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
            glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

            // Set up and enable attribute 1
            glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(vao, 0, 0);
            glEnableVertexArrayAttrib(vao, 0);
        }

        void drawSquare(const glm::vec3& position, const glm::vec3& color,
                        float rotation = 0.0f, const glm::vec3& scale = glm::vec3(1.0f)) {
            glm::mat4 transform = glm::scale(scale);
            transform = glm::translate(transform, position);
            transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            basicShader.uploadVec3("uColor", color);
            basicShader.uploadMat4("uTransform", transform);
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        static glm::vec3 playerPos = glm::vec3(3.0f, 1.5f, 0.0f);
        static glm::vec3 playerSpeed = glm::vec3(0.8f, 0.8f, 0.0f);
        static glm::vec3 playerColor = glm::vec3(230.0f/255.0f, 159.0f/255.0f, 102.0f/255.0f);
        static float playerRotation = 0.0f;
        static float keyDebounce = 0.0f;
        glm::mat4 projection;

        void init(const Window& window) {
            // Load and compile shader
            if(!basicShader.compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/basic.glsl")) {
                basicShader.destroy();
                g_logger_error("Failed to compile the shader program, exiting early.");
            }

            initSquare();

            float projectionWidth = 6.0f;
            float windowAspect = ((float)window.width / (float)window.height);
            float projectionHeight = projectionWidth / windowAspect;
            projection = glm::ortho(0.0f, projectionWidth, 0.0f, projectionHeight, 0.0001f, 10000.0f);
        }

        void destroy() {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
        }

        void update(float dt) {
            keyDebounce -= dt;
            basicShader.bind();

            glm::vec3 eye = glm::vec3(0.0f, 0.0f, 20.0f);
            glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::mat4 view = glm::lookAt(eye, center, up);
            basicShader.uploadMat4("uView", view);
            basicShader.uploadMat4("uProjection", projection);

            static bool rotate = false;
            if(rotate) {
                playerRotation += dt * 40.0f;
            }
            drawSquare(playerPos, playerColor, playerRotation);

            if(Input::isKeyDown(GLFW_KEY_E) && keyDebounce < 0) {
                rotate = !rotate;
                keyDebounce = 0.3f;
            }

            if(Input::isKeyDown(GLFW_KEY_D)) {
                playerPos.x += dt * playerSpeed.x;
            } else if(Input::isKeyDown(GLFW_KEY_A)) {
                playerPos.x -= dt * playerSpeed.x;
            }

            if(Input::isKeyDown(GLFW_KEY_W)) {
                playerPos.y += dt * playerSpeed.y;
            } else if(Input::isKeyDown(GLFW_KEY_S)) {
                playerPos.y -= dt * playerSpeed.y;
            }
        }
    }
}
