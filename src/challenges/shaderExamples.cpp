#include "shaderExamples.h"
#include "../app.h"
#include "../core/input.h"
#include "../core/window.h"
#include "../renderer/shaderProgram.h"

namespace GLTest {
    namespace ShaderExamples {
        struct Vertex {
            glm::vec3 position;
        };

        const int numShaders = 9;
        const int lastShader = numShaders - 1;

        // static std::array<Vertex, 4> vertices {
        //     Vertex{glm::vec3(-0.5f, -0.5f, 0.0f)}, // Bottom left
        //     Vertex{glm::vec3(-0.5f,  0.5f, 0.0f)}, // Top left
        //     Vertex{glm::vec3( 0.5f,  0.5f, 0.0f)}, // Top right
        //     Vertex{glm::vec3( 0.5f, -0.5f, 0.0f)}, // Bottom right
        // };

        static std::array<Vertex, 4> vertices {
            Vertex{glm::vec3(-2.0f, -0.7f, 0.0f)}, // Bottom left
            Vertex{glm::vec3(-2.0f,  0.7f, 0.0f)}, // Top left
            Vertex{glm::vec3( 2.0f,  0.7f, 0.0f)}, // Top right
            Vertex{glm::vec3( 2.0f, -0.7f, 0.0f)}, // Bottom right
        };

        static std::array<u32, 6> indices {0, 1, 2, 0, 2, 3};

        static Vertex* tVerts = nullptr;
        static u32* tIndices = nullptr;
        static u32 vao, tVao;
        static u32 vbo, tVbo;
        static u32 ebo, tEbo;

        static ShaderProgram shaders[numShaders];
        static int currentShaderIndex;

        static int numElements = 0;
        static int numVertices = 0;
        static int maxSubdivision = 20;
        constexpr float terrainSize = 10.0f;

        static void calculateVerts();

        void initQuad() {
            glCreateVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Setup and buffer the element buffer
            glCreateBuffers(1, &ebo);
            glNamedBufferData(ebo, sizeof(indices), indices.data(), GL_STATIC_DRAW);
            glVertexArrayElementBuffer(vao, ebo);

            // Setup and buffer the vertex buffer
            glCreateBuffers(1, &vbo);
            glNamedBufferData(vbo, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
            glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

            // Setup and enable attribute 1
            glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(vao, 0, 0);
            glEnableVertexArrayAttrib(vao, 0);
        }

        void initPlane() {
            numVertices = 0;
            numElements = 0;

            calculateVerts();
            glCreateVertexArrays(1, &tVao);
            glBindVertexArray(tVao);

            // Setup and buffer the element buffer
            glCreateBuffers(1, &tEbo);
            glNamedBufferData(tEbo, sizeof(u32) * numElements, tIndices, GL_STATIC_DRAW);
            glVertexArrayElementBuffer(tVao, tEbo);

            // Setup and buffer the vertex buffer
            glCreateBuffers(1, &tVbo);
            glNamedBufferData(tVbo, sizeof(Vertex) * numVertices, tVerts, GL_STATIC_DRAW);
            glVertexArrayVertexBuffer(tVao, 0, tVbo, 0, sizeof(Vertex));

            // Setup and enable attribute 1
            glVertexArrayAttribFormat(tVao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(tVao, 0, 0);
            glEnableVertexArrayAttrib(tVao, 0);
        }

        void drawQuad(const ShaderProgram& shader, const glm::vec3& position, 
                      const glm::vec3& color, float rotation=0.0f, 
                      const glm::vec3& scale=glm::vec3(1.0f)) {
            glm::mat4 transform = glm::scale(scale);
            transform = glm::translate(transform, position);
            transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

            if(currentShaderIndex == 0) {
                shader.uploadVec3("u_color", color);
                shader.uploadMat4("u_transform", transform);
            } else {
                shader.uploadMat4("u_transform", transform);
                shader.uploadVec2("u_resolution", glm::vec2(1.0f));
                glm::vec2 localMouse = glm::vec2(Input::mouseX, Input::mouseY);
                shader.uploadVec2("u_mouse", localMouse);
            }

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        void drawPlane(const ShaderProgram& shader) {
            shader.uploadMat4("u_transform", glm::identity<glm::mat4>());
            shader.uploadVec2("u_resolution", glm::vec2(5.0f));

            glBindVertexArray(tVao);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
        }

        void destroyQuad() {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
        }

        void destroyPlane() {
            glDeleteBuffers(1, &tVbo);
            glDeleteBuffers(1, &tEbo);
            glDeleteVertexArrays(1, &tVao);
            g_memory_free(tVerts);
            g_memory_free(tIndices);
        }

        static glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 0.0f);
        static glm::vec3 playerSpeed = glm::vec3(0.8f, 0.8f, 0.0f);
        static glm::vec3 playerColor = glm::vec3(230.0f/255.0f, 159.0f/255.0f, 102.0f/255.0f);
        static float playerRotation = 0.0f;
        glm::mat4 projection;
        glm::mat4 tProjection;

        void init(const Window& window) {
            // Load shaders
            shaders[0].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/basic.glsl");
            shaders[1].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/pulsingCircles.glsl");
            shaders[2].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/mondrian.glsl");
            shaders[3].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/coolCircle.glsl");
            shaders[4].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/spinningWheel.glsl");
            shaders[5].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/babushka.glsl");
            shaders[6].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/Above.the.Clouds.glsl");
            shaders[7].compileAndLink("assets/shaders/vertex/basic.glsl", "assets/shaders/fragment/Dancing.Dots.glsl");
            shaders[8].compileAndLink("assets/shaders/vertex/terrain.glsl", "assets/shaders/fragment/terrain.glsl");

            initQuad();
            initPlane();

            float windowAspect = ((float)window.width / (float)window.height);
            tProjection = glm::perspective(45.0f, windowAspect, 0.01f, 10000.0f);
            
            float projectionWidth = 6.0f;
            float projectionHeight = projectionWidth / windowAspect;
            projection = glm::ortho(-projectionWidth/2.0f, projectionWidth/2.0f, 
                                    -projectionHeight/2.0f, projectionHeight/2.0f, 
                                    0.001f, 100000.0f);
        }

        void destroy() {
            destroyQuad();
            destroyPlane();

            for(int i=0; i<numShaders; i++) {
                shaders[i].destroy();
            }
        }

        void update(float dt) {
            static float keyDebounce = 0.0f;
            keyDebounce -= dt;
            shaders[currentShaderIndex].bind();

            static float rotation = 0.0f;
            if(currentShaderIndex == lastShader) {
                rotation += dt * 30.0f;
            } else {
                rotation = 0.0f;
            }
            float rotationRadians = glm::radians(rotation);

            glm::vec3 eye = glm::vec3(20.0f * glm::sin(rotationRadians), 0.0f, 20.0f * glm::cos(rotationRadians));
            if(currentShaderIndex == 0) {
                eye = glm::vec3(0.0f, 0.0f, 20.0f);
            }
            glm::vec3 center = glm::vec3(0.0f);
            glm::mat4 uProjection = projection;
            if(currentShaderIndex == lastShader) {
                uProjection = tProjection;
                constexpr float maxRadius = (terrainSize / 2.0f) * 1.8f;
                eye = glm::vec3(maxRadius * glm::sin(rotationRadians), 6.0f, maxRadius * glm::cos(rotationRadians));
                center = glm::vec3(0.0f, -2.0f, 0.0f);
                shaders[currentShaderIndex].uploadFloat("u_gridSize", terrainSize / (float)maxSubdivision);
            }

            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::mat4 view = glm::lookAt(eye, center, up);
            shaders[currentShaderIndex].uploadMat4("u_view", view);
            shaders[currentShaderIndex].uploadMat4("u_projection", uProjection);
            shaders[currentShaderIndex].uploadFloat("u_time", (float)glfwGetTime());

            static bool rotate = false;
            if(rotate) {
                playerRotation += dt * 40.0f;
            }

            if(currentShaderIndex == 0) {
                drawQuad(shaders[currentShaderIndex], playerPos, playerColor, playerRotation);
            } else if(currentShaderIndex == lastShader) {
                drawPlane(shaders[currentShaderIndex]);
            } else {
                drawQuad(shaders[currentShaderIndex], glm::vec3(0.0f), glm::vec3(1.0f), 0.0f, glm::vec3(3.0f));
            }

            // Change shader
            if(Input::isKeyDown(GLFW_KEY_1) && keyDebounce < 0.0f) {
                currentShaderIndex = 0;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_2) && keyDebounce < 0.0f) {
                currentShaderIndex = 1;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_3) && keyDebounce < 0.0f) {
                currentShaderIndex = 2;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_4) && keyDebounce < 0.0f) {
                currentShaderIndex = 3;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_5) && keyDebounce < 0.0f) {
                currentShaderIndex = 4;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_6) && keyDebounce < 0.0f) {
                currentShaderIndex = 5;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_7) && keyDebounce < 0.0f) {
                currentShaderIndex = 6;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_8) && keyDebounce < 0.0f) {
                currentShaderIndex = 7;
                keyDebounce = 0.3f;
            } else if(Input::isKeyDown(GLFW_KEY_9) && keyDebounce < 0.0f) {
                currentShaderIndex = 8;
                keyDebounce = 0.3f;
            }

            if(currentShaderIndex == 0) {
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
            } else if(currentShaderIndex == lastShader) {
                if(Input::isKeyDown(GLFW_KEY_UP) && keyDebounce < 0.0f) {
                    maxSubdivision++;
                    destroyPlane();
                    initPlane();
                    keyDebounce = 0.3f;
                } else if(Input::isKeyDown(GLFW_KEY_DOWN) && keyDebounce < 0.0f) {
                    maxSubdivision--;
                    maxSubdivision = glm::max(1, maxSubdivision);
                    destroyPlane();
                    initPlane();
                    keyDebounce = 0.3f;
                }
            }
        }

        static void calculateVerts() {
            constexpr float leftX = -(terrainSize / 2.0f);
            constexpr float bottomY = -(terrainSize / 2.0f);
            float gridSize = terrainSize / (float)maxSubdivision;

            numVertices = maxSubdivision * maxSubdivision;
            tVerts = (Vertex*)g_memory_allocate(sizeof(Vertex) * numVertices);
            for(int y=0; y<maxSubdivision; y++) {
                for(int x=0; x<maxSubdivision; x++) {
                    tVerts[x + (y * maxSubdivision)].position.x = (leftX + 0.5f) + (gridSize * x);
                    tVerts[x + (y * maxSubdivision)].position.y = -2.0f;
                    tVerts[x + (y * maxSubdivision)].position.z = (bottomY + 0.5f) + (gridSize * y);
                }
            }

            int numQuads = (maxSubdivision - 1) * (maxSubdivision - 1);
            numElements = numQuads * 6;
            tIndices = (uint32*)g_memory_allocate(sizeof(u32) * numElements);
            for(int i=0; i<numQuads; i++) {
                int quadIndex = i + (i / (maxSubdivision - 1));
                // First triangle
                tIndices[(i * 6) + 0] = quadIndex;
                tIndices[(i * 6) + 1] = quadIndex + 1;
                tIndices[(i * 6) + 2] = quadIndex + maxSubdivision;

                // Second triangle
                tIndices[(i * 6) + 3] = quadIndex + maxSubdivision;
                tIndices[(i * 6) + 4] = quadIndex + 1;
                tIndices[(i * 6) + 5] = quadIndex + maxSubdivision + 1;
            }
        }
    }
}
