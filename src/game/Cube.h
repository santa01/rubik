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

#include "NonCopyable.h"
#include "CubePart.h"
#include "MeshData.h"

#include <array>

namespace Rubik {

namespace Game {

class Cube: public Common::Renderable, public Common::NonCopyable {
public:
    static const Math::Vec3 DUMMY_SELECTION;

    enum CubeState {
        STATE_IDLE,
        STATE_LEFT_ROTATION,
        STATE_RIGHT_ROTATION,
        STATE_UP_ROTATION,
        STATE_DOWN_ROTATION
    };

    Cube() {
        for (int i = 0; i < 27; i++) {
            this->cubeParts[i] = std::shared_ptr<CubePart>(new CubePart());
            this->cubeParts[i]->setPosition(i / 9 - 1, i % 9 / 3 - 1, i % 9 % 3 - 1);
            this->cubeParts[i]->scale(0.5f);
            this->cubeParts[i]->setId(i);
        }

        this->rotationSpeed = 300.0f;
        this->state = STATE_IDLE;
    }

    float getRotationSpeed() const {
        return this->rotationSpeed;
    }

    void setRotationSpeed(float rotationSpeed) {
        this->rotationSpeed = rotationSpeed;
    }

    const Math::Vec3& getSelectedSubCube() const {
        return this->selectedSubCube;
    }

    void selectSubCube(const Math::Vec3& cubeArrayPosition) {
        this->selectedSubCube = cubeArrayPosition;
    }

    CubeState getState() const {
        return this->state;
    }

    void setState(CubeState state) {
        this->state = state;
    }

    std::shared_ptr<Opengl::RenderEffect>& getEffect() {
        return this->cubeParts[0]->getEffect();
    }

    void setEffect(const std::shared_ptr<Opengl::RenderEffect>& effect) {
        for (auto& cubePart: this->cubeParts) {
            cubePart->setEffect(effect);
        }
    }

    std::shared_ptr<Opengl::Texture>& getTexture() {
        return this->cubeParts[0]->getTexture();
    }

    void setTexture(const std::shared_ptr<Opengl::Texture>& texture) {
        for (auto& cubePart: this->cubeParts) {
            cubePart->setTexture(texture);
        }
    }

    void render();

    bool load(const std::shared_ptr<Utils::MeshData>& meshData) {
        for (auto& cubePart: this->cubeParts) {
            if (!cubePart->load(meshData)) {
                return false;
            }
        }
        return true;
    }

    bool inOrder() const {
        for (int i = 0; i < 27; i++) {
            if (this->cubeParts[i]->getId() != i) {
                return false;
            }
        }
        return true;
    }

    void shuffle(int times);
    void animate(float frameTime);

private:
    void rotateCubeArray(CubeState state, const Math::Vec3 cubeArrayPosition);
    void rotate(CubeState state, float angle, const Math::Vec3 cubeArrayPosition);

    std::array<std::shared_ptr<CubePart>, 27> cubeParts;

    Math::Vec3 selectedSubCube;
    CubeState state;
    float rotationSpeed;
};

}  // namespace Game

}  // namespace Rubik

#endif  // CUBE_H
