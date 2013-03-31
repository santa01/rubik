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

#include "CubeMesh.h"
#include "ResourceManager.h"

namespace Rubik {

namespace Opengl {

const float CubeMesh::vertices[] = {
    // coords
     1.0f,  1.0f, -1.0f,  // 0
    -1.0f,  1.0f, -1.0f,  // 1
    -1.0f,  1.0f,  1.0f,  // 2
    -1.0f,  1.0f, -1.0f,  // 3
    -1.0f, -1.0f, -1.0f,  // 4
    -1.0f, -1.0f,  1.0f,  // 5
    -1.0f, -1.0f, -1.0f,  // 6
    -1.0f,  1.0f, -1.0f,  // 7
     1.0f,  1.0f, -1.0f,  // 8
     1.0f, -1.0f, -1.0f,  // 9
     1.0f, -1.0f,  1.0f,  // 10
     1.0f, -1.0f, -1.0f,  // 11
     1.0f,  1.0f, -1.0f,  // 12
     1.0f,  1.0f,  1.0f,  // 13
     // UVs
     0.5f,   0.0f,   // 0
     0.25f,  0.0f,   // 1
     0.25f,  0.25f,  // 2
     0.0f,   0.25f,  // 3
     0.0f,   0.5f,   // 4
     0.25f,  0.5f,   // 5
     0.25f,  0.75f,  // 6
     0.25f,  1.0f,   // 7
     0.5f,   1.0f,   // 8
     0.5f,   0.75f,  // 9
     0.5f,   0.5f,   // 10
     0.75f,  0.5f,   // 11
     0.75f,  0.25f,  // 12
     0.5f,   0.25f,  // 12
};

const int CubeMesh::indices[] = {
    13, 1,  0,   // 0
    13, 2,  1,   // 1
    5,  3,  2,   // 2
    5,  4,  3,   // 3
    10, 2,  13,  // 4
    10, 5,  2,   // 5
    11, 13, 12,  // 6
    11, 10, 13,  // 7
    9,  5,  10,  // 8
    9,  6,  5,   // 9
    8,  6,  9,   // 10
    8,  7,  6,   // 11
};

CubeMesh::CubeMesh() {
    glGenBuffers(2, this->buffers);
    glGenVertexArrays(1, &this->vao);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->buffers[VERTEX_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(this->vertices) * 3 / 5));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers[ELEMENT_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices), this->indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    this->xAngle = 0.0f;
    this->yAngle = 0.0f;
    this->zAngle = 0.0f;

    this->id = 0;
}

void CubeMesh::rotate(const Math::Vec3& vector, float angle) {
    if (vector == Math::Vec3::ZERO) {
        return;
    }

    Math::Vec3 axis(vector);
    Math::Quaternion q(axis.normalize(), angle * M_PI / 180.0f);
    q.normalize();

    this->rotation = q.extractMat4() * this->rotation;

    float xAngleNew, yAngleNew, zAngleNew;
    q.extractEulerAgngles(xAngleNew, yAngleNew, zAngleNew);

    this->xAngle += xAngleNew * 180.f / M_PI;
    this->yAngle += yAngleNew * 180.f / M_PI;
    this->zAngle += zAngleNew * 180.f / M_PI;
}

void CubeMesh::render() {
    if (this->texture != nullptr) {
        this->texture->bind();
    }

    if (this->effect != nullptr) {
        this->effect->enable();
        this->effect->setUniform("lw", this->rotation * this->translation * this->scaling);
    }

    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, sizeof(this->indices) / sizeof(int), GL_UNSIGNED_INT, (GLvoid*)0);
    glBindVertexArray(0);
}

}  // namespace Opengl

}  // namespace Rubik
