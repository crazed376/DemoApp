#include "terrain.h"
#include "../app.h"
#include "../core/input.h"
#include "../core/window.h"
#include "../renderer/shaderProgram.h"

namespace GLTest {
    namespace TerrainShader {
        struct Vertex {
            glm::vec3 position;
        };

        static Vertex* vertices = nullptr;
        static u32* indices = nullptr;
        static int numElements = 0;
        static int numVertices = 0;

        static u32 vao;
        static u32 vbo;
        static u32 ebo;

        static ShaderProgram shaderProgram;

        static int maxSubdivision = 20;
        constexpr float terrainSize = 10.0f;

        static void calculateVerts();

        void initPlane() {
            numVertices = 0;
            numElements = 0;

            calculateVerts();
            glCreateVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Setup and buffer the element buffer
            glCreateBuffers(1, &ebo);
            glNamedBufferData(ebo, sizeof(u32) * numElements, indices, GL_STATIC_DRAW);
            glVertexArrayElementBuffer(vao, ebo);

            // Setup and buffer the vertex buffer
            glCreateBuffers(1, &vbo);
            glNamedBufferData(vbo, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW);
            glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

            // Setup and enable attribute 1
            glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
            glVertexArrayAttribBinding(vao, 0, 0);
            glEnableVertexArrayAttrib(vao, 0);
        }

        void drawPlane(const ShaderProgram& shader) {
            shader.uploadMat4("u_transform", glm::identity<glm::mat4>());
            shader.uploadVec2("u_resolution", glm::vec2(5.0f));

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
        }

        void destroyPlane() {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
            g_memory_free(vertices);
            g_memory_free(indices);
        }

        glm::mat4 projection;

        void init(const Window& window) {
            shaderProgram.compileAndLink("assets/shaders/vertex/terrain.glsl", "assets/shaders/fragment/terrain.glsl");

            initPlane();

            float windowAspect = ((float)window.width / (float)window.height);
            projection = glm::perspective(45.0f, windowAspect, 0.01f, 10000.0f);
        }

        void destroy() {
            destroyPlane();
            shaderProgram.destroy();
        }

        void update(float dt) {
            static float keyDebounce = 0.0f;
            keyDebounce -= dt;
            shaderProgram.bind();

            static float rotation = 0.0f;
            rotation += dt * 30.0f;
            float rotationRadians = glm::radians(rotation);

            constexpr float maxRadius = (terrainSize / 2.0f) * 1.8f;
            glm::vec3 eye = glm::vec3(maxRadius * glm::sin(rotationRadians), 6.0f, maxRadius * glm::cos(rotationRadians));
            glm::vec3 center = glm::vec3(0.0f, -2.0f, 0.0f);
            shaderProgram.uploadFloat("u_gridSize", terrainSize / (float)maxSubdivision);

            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::mat4 view = glm::lookAt(eye, center, up);
            shaderProgram.uploadMat4("u_view", view);
            shaderProgram.uploadMat4("u_projection", projection);
            shaderProgram.uploadFloat("u_time", (float)glfwGetTime());

        
            drawPlane(shaderProgram);

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

        static void calculateVerts() {
            constexpr float leftX = -(terrainSize / 2.0f);
            constexpr float bottomY = -(terrainSize / 2.0f);
            float gridSize = terrainSize / (float)maxSubdivision;

            numVertices = maxSubdivision * maxSubdivision;
            vertices = (Vertex*)g_memory_allocate(sizeof(Vertex) * numVertices);
            for(int y=0; y<maxSubdivision; y++) {
                for(int x=0; x<maxSubdivision; x++) {
                    vertices[x + (y * maxSubdivision)].position.x = (leftX + 0.5f) + (gridSize * x);
                    vertices[x + (y * maxSubdivision)].position.y = -2.0f;
                    vertices[x + (y * maxSubdivision)].position.z = (bottomY + 0.5f) + (gridSize * y);
                }
            }

            int numQuads = (maxSubdivision - 1) * (maxSubdivision - 1);
            numElements = numQuads * 6;
            indices = (uint32*)g_memory_allocate(sizeof(u32) * numElements);
            for(int i=0; i<numQuads; i++) {
                int quadIndex = i + (i / (maxSubdivision - 1));
                // First triangle
                indices[(i * 6) + 0] = quadIndex;
                indices[(i * 6) + 1] = quadIndex + 1;
                indices[(i * 6) + 2] = quadIndex + maxSubdivision;

                // Second triangle
                indices[(i * 6) + 3] = quadIndex + maxSubdivision;
                indices[(i * 6) + 4] = quadIndex + 1;
                indices[(i * 6) + 5] = quadIndex + maxSubdivision + 1;
            }
        }
    }
}
