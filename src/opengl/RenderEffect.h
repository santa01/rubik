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

#include "NonCopyable.h"
#include "ShaderLoader.h"

#include <GL/glew.h>
#include <Mat4.h>
#include <Mat3.h>
#include <Vec4.h>
#include <Vec3.h>
#include <unordered_map>
#include <utility>
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

    void setUniform(const std::string& name, const Math::Mat4& value) {
        GLint uniform = this->checkoutUniform(name);
        if (uniform > -1) {
            glUniformMatrix4fv(uniform, 1, GL_TRUE, (GLfloat*)value.data());
        }
    }

    void setUniform(const std::string& name, const Math::Mat3& value) {
        GLint uniform = this->checkoutUniform(name);
        if (uniform > -1) {
            glUniformMatrix3fv(uniform, 1, GL_TRUE, (GLfloat*)value.data());
        }
    }

    void setUniform(const std::string& name, const Math::Vec4& value) {
        GLint uniform = this->checkoutUniform(name);
        if (uniform > -1) {
            glUniform4fv(uniform, 1, (GLfloat*)value.data());
        }
    }

    void setUniform(const std::string& name, const Math::Vec3& value) {
        GLint uniform = this->checkoutUniform(name);
        if (uniform > -1) {
            glUniform3fv(uniform, 1, (GLfloat*)value.data());
        }
    }

    void setUniform(const std::string& name, float value) {
        GLint uniform = this->checkoutUniform(name);
        if (uniform > -1) {
            glUniform1f(uniform, value);
        }
    }

    void setUniform(const std::string& name, int value) {
        GLint uniform = this->checkoutUniform(name);
        if (uniform > -1) {
            glUniform1i(uniform, value);
        }
    }

    void attachShader(const std::string& source, ShaderType type) {
        if (this->program == 0) {
            this->shaderList.push_back(Opengl::ShaderLoader::createShader(source, type));
        }
    }

    void enable() {
        if (this->program == 0) {
            this->program = Opengl::ShaderLoader::createProgram(this->shaderList);
        }

        glUseProgram(this->program);
    }

private:
    GLint checkoutUniform(const std::string& name) {
        this->enable();

        if (this->uniforms.find(name) == this->uniforms.end()) {
            this->uniforms.insert(std::make_pair(name, glGetUniformLocation(this->program, name.c_str())));
        }

        return this->uniforms.at(name);
    }

    std::unordered_map<std::string, GLint> uniforms;
    std::vector<GLuint> shaderList;

    GLuint program;
};

}  // namespace Opengl

}  // namespace Rubik

#endif  // RENDEREFFECT_H
