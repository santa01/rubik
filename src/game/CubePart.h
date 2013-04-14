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

#ifndef CUBEPART_H
#define CUBEPART_H

#include "Mesh.h"
#include "Movable.h"
#include "Scalable.h"
#include "Rotatable.h"
#include "Renderable.h"
#include "RenderEffect.h"
#include "Texture.h"

namespace Rubik {

namespace Game {

class CubePart: public Common::Movable, public Common::Rotatable, public Common::Scalable, public Common::Renderable {
public:
    CubePart():
            cubeMesh(new Opengl::Mesh()) {
        this->id = 0;
    }

    int getId() const {
        return this->id;
    }

    void setId(int id) {
        this->id = id;
    }

    using Common::Movable::setPosition;

    void setPosition(const Math::Vec3& position) {
        this->cubeMesh->setPosition(position);
    }

    Math::Vec3 getPosition() const {
        return this->cubeMesh->getPosition();
    }

    float getXAngle() const {
        return this->cubeMesh->getXAngle();
    }

    float getYAngle() const {
        return this->cubeMesh->getYAngle();
    }

    float getZAngle() const {
        return this->cubeMesh->getZAngle();
    }

    void rotate(const Math::Vec3& vector, float angle) {
        this->cubeMesh->rotate(vector, angle);
    }

    void scaleX(float factor) {
        this->cubeMesh->scaleX(factor);
    }

    void scaleY(float factor) {
        this->cubeMesh->scaleY(factor);
    }

    void scaleZ(float factor) {
        this->cubeMesh->scaleZ(factor);
    }

    float getXFactor() const {
        return this->cubeMesh->getXFactor();
    }

    float getYFactor() const {
        return this->cubeMesh->getYFactor();
    }

    float getZFactor() const {
        return this->cubeMesh->getZFactor();
    }

    std::shared_ptr<Opengl::RenderEffect>& getEffect() {
        return this->cubeMesh->getEffect();
    }

    void setEffect(const std::shared_ptr<Opengl::RenderEffect>& effect) {
        this->cubeMesh->setEffect(effect);
    }

    std::shared_ptr<Opengl::Texture>& getTexture() {
        return this->cubeMesh->getTexture();
    }

    void setTexture(const std::shared_ptr<Opengl::Texture>& texture) {
        this->cubeMesh->setTexture(texture);
    }

    void render() {
        this->cubeMesh->render();
    }

    bool load(const std::shared_ptr<Utils::MeshData>& vertexData) {
        return this->cubeMesh->load(vertexData);
    }

private:
    std::shared_ptr<Opengl::Mesh> cubeMesh;

    int id;
};

}  // namespace Game

}  // namespace Rubik

#endif  // CUBEPART_H
