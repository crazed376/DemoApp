#ifndef SHADER_EXAMPLE_H
#define SHADER_EXAMPLE_H

namespace GLTest {
    struct Window;

    namespace ShaderExamples {
        void init(const Window& window);
        void destroy();

        void update(float dt);
    }
}

#endif
