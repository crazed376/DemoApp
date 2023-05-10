#ifndef SHADER_SQUARE_H
#define SHADER_SQUARE_H

namespace GLTest {
    struct Window;

    namespace ShaderSquare {
        void init(const Window& window);
        void destroy();

        void update(float dt);
    }
}

#endif
