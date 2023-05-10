#ifndef WORLD_H_
#define WORLD_H_

#include "chunk.h" // Needed?

namespace GLTest {
    struct Window;

    namespace World {
        void init(const Window& window);
        void destroy();

        void update(float dt);
    }
}

#endif