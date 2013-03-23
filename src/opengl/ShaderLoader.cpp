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

#include "ShaderLoader.h"
#include "Logger.h"

#include <fstream>
#include <memory>

namespace Rubik {

namespace Opengl {

GLuint ShaderLoader::createProgram(const std::vector<GLuint>& shaders) {
    GLuint program = glCreateProgram();

    for (auto& shader: shaders) {
        glAttachShader(program, shader);
        glDeleteShader(shader);
    }

    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogLength]);
        glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.get());

        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "%s", infoLog.get());
    }

    return program;
}

GLuint ShaderLoader::createShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const GLchar* sourceStrings = source.c_str();

    glShaderSource(shader, 1, &sourceStrings, nullptr);
    glCompileShader(shader);

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogLength]);
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.get());

        Utils::Logger::getInstance().log(Utils::Logger::LOG_ERROR, "%s", infoLog.get());
    }

    return shader;
}

}  // namespace Opengl

}  // namespace Rubik
