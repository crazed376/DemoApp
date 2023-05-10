#ifndef CUBE_H
#define CUBE_H

namespace GLTest {
    struct Window;

    namespace Cubes {
        void init(const Window& window);
        void destroy();

        void update(float dt);
    }
}

#endif
