#ifndef SHADER_H
#define SHADER_H

#include "../app.h"

namespace GLTest {
    enum class ShaderType: u8 {
        Vertex,
        Fragment,
    };

    struct Shader {
        char* filepath;
        u32 shaderId;
        ShaderType type;

        bool compile(ShaderType type, const char* shaderFilepath);
        void destroy();

        static GLenum toGlShaderType(ShaderType type);
    };
}

#endif
