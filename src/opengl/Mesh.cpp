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

#include "Mesh.h"
#include "Quaternion.h"

namespace Rubik {

namespace Opengl {

void Mesh::rotate(const Math::Vec3& vector, float angle) {
    if (vector == Math::Vec3::ZERO) {
        return;
    }

    Math::Vec3 axis(vector);
    Math::Quaternion q(axis.normalize(), angle * M_PI / 180.0f);
    q.normalize();

    this->rotation = q.extractMat4() * this->rotation;

    float xAngleNew, yAngleNew, zAngleNew;
    q.extractEulerAngles(xAngleNew, yAngleNew, zAngleNew);

    this->xAngle += xAngleNew * 180.f / M_PI;
    this->yAngle += yAngleNew * 180.f / M_PI;
    this->zAngle += zAngleNew * 180.f / M_PI;
}

bool Mesh::load(const std::shared_ptr<Utils::MeshData>& vertexData) {
    if (vertexData == nullptr || vertexData->getFacesData() == nullptr || vertexData->getVerticesData() == nullptr) {
        return false;
    }

    this->vertexData = vertexData;
    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->buffers[VERTEX_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, this->vertexData->getVerticesNumber() * sizeof(float) * 5, // vertices + uvs
            this->vertexData->getVerticesData(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
            (GLvoid*)(this->vertexData->getVerticesNumber() * sizeof(float) * 3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers[ELEMENT_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->vertexData->getFacesNumber() * sizeof(int) * 3,
            this->vertexData->getFacesData(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    return true;
}

void Mesh::render() {
    if (this->texture != nullptr) {
        this->texture->bind();
    }

    if (this->effect != nullptr) {
        this->effect->enable();
        this->effect->setUniform("lw", this->rotation * this->translation * this->scaling);
    }

    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, this->vertexData->getFacesNumber() * 3, GL_UNSIGNED_INT, (GLvoid*)0);
    glBindVertexArray(0);
}

}  // namespace Opengl

}  // namespace Rubik
