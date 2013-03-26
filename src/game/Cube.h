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

#ifndef CUBE_H
#define CUBE_H

#include "Rotatable.h"
#include "NonCopyable.h"
#include "CubeMesh.h"
#include "RenderEffect.h"

#include <memory>
#include <array>

namespace Rubik {

namespace Game {

class Cube: public Common::Rotatable, public Common::NonCopyable {
public:
    enum CubeState {
        STATE_IDLE,
        STATE_LEFT_ROTATION,
        STATE_RIGHT_ROTATION,
        STATE_UP_ROTATION,
        STATE_DOWN_ROTATION
    };

    Cube();

    CubeState getState() const {
        return this->state;
    }

    void setState(CubeState state) {
        if (this->state == STATE_IDLE) {
            this->state = state;
        }
    }

    float getXAngle() const {
        return this->cubeArray[0]->getXAngle();
    }

    float getYAngle() const {
        return this->cubeArray[0]->getYAngle();
    }

    float getZAngle() const {
        return this->cubeArray[0]->getZAngle();
    }

    void rotate(const Math::Vec3& vector, float angle) {
        for (auto& subCube: this->cubeArray) {
            subCube->rotate(vector, angle);
        }
    }

    std::shared_ptr<Opengl::RenderEffect>& getEffect() {
        return this->cubeArray[0]->getEffect();
    }

    void setEffect(const std::shared_ptr<Opengl::RenderEffect>& effect) {
        for (auto& subCube: this->cubeArray) {
            subCube->setEffect(effect);
        }
    }

    std::shared_ptr<Opengl::Texture>& getTexture() {
        return this->cubeArray[0]->getTexture();
    }

    void setTexture(const std::shared_ptr<Opengl::Texture>& texture) {
        for (auto& subCube: this->cubeArray) {
            subCube->setTexture(texture);
        }
    }

    void render() {
        for (auto& subCube: this->cubeArray) {
            subCube->render();
        }
    }

    void animate(float frameTime);

private:
    std::array<std::unique_ptr<Opengl::CubeMesh>, 27> cubeArray;

    Math::Vec3 verticalAxis;
    Math::Vec3 horizontalAxis;

    CubeState state;
};

}  // namespace Game

}  // namespace Rubik

#endif  // CUBE_H
