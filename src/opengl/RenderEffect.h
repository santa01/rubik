/*
 * Copyright (c) 2013 Pavlo Lavrenenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RENDEREFFECT_H
#define RENDEREFFECT_H

#include "Mat4.h"
#include "Vec3.h"
#include "NonCopyable.h"
#include "ShaderLoader.h"

#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <string>

namespace Rubik {

namespace Opengl {

class RenderEffect: public Common::NonCopyable {
public:
    enum ShaderType {
        TYPE_VERTEX = GL_VERTEX_SHADER,
        TYPE_FRAGMENT = GL_FRAGMENT_SHADER
    };

    RenderEffect() {
        this->program = 0;
    }

    ~RenderEffect() {
        if (this->program != 0) {
            glDeleteProgram(this->program);
        }
    }

    void setMVP(const Math::Mat4& matrix) {
        this->enable();

        if (this->mvp > -1) {
            glUniformMatrix4fv(this->mvp, 1, GL_TRUE, (GLfloat*)matrix.data());
        }
    }

    // local -> world space transformation matrix
    void setLW(const Math::Mat4& matrix) {
        this->enable();

        if (this->lw > -1) {
            glUniformMatrix4fv(this->lw, 1, GL_TRUE, (GLfloat*)matrix.data());
        }
    }

    void attachShader(const std::string& source, ShaderType type) {
        if (this->program == 0) {
            this->shaderList.push_back(Opengl::ShaderLoader::createShader(source, type));
        }
    }

    void enable();

private:
    std::vector<GLuint> shaderList;

    GLuint program;

    GLint mvp;        // vertex shader
    GLint lw;         // vertex shader
};

}  // namespace Opengl

}  // namespace Rubik

#endif  // RENDEREFFECT_H
