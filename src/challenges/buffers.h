#ifndef BUFFER_H
#define BUFFER_H

namespace GLTest {
    struct Window;

    namespace BufferChallenge {
        void init(const Window& window);
        void destroy();

        void update(float dt);
    }
}

#endif
