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

#include <ctime>

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
        int id = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    auto cubePart = std::shared_ptr<CubePart>(new CubePart());
                    cubePart->setPosition(i - 1, j - 1, k - 1);  // Center cube goes to (0; 0; 0)
                    cubePart->scale(0.5f);
                    cubePart->setId(id++);
                    this->cubeParts[i][j][k] = cubePart;
                }
            }
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
        return this->selectedCubePart;
    }

    void selectSubCube(const Math::Vec3& cubeArrayPosition) {
        this->selectedCubePart = cubeArrayPosition;
    }

    CubeState getState() const {
        return this->state;
    }

    void setState(CubeState state) {
        this->state = state;
    }

    std::shared_ptr<Opengl::RenderEffect>& getEffect() {
        return this->cubeParts[0][0][0]->getEffect();
    }

    void setEffect(const std::shared_ptr<Opengl::RenderEffect>& effect) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    this->cubeParts[i][j][k]->setEffect(effect);
                }
            }
        }
    }

    std::shared_ptr<Opengl::Texture>& getTexture() {
        return this->cubeParts[0][0][0]->getTexture();
    }

    void setTexture(const std::shared_ptr<Opengl::Texture>& texture) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    this->cubeParts[i][j][k]->setTexture(texture);
                }
            }
        }
    }

    void render() {
        auto& renderEffect = this->getEffect();
        if (renderEffect == nullptr) {
            return;
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    renderEffect->setUniform("cubeArrayPosition", Math::Vec3(i, j, k));
                    this->cubeParts[i][j][k]->render();
                }
            }
        }
    }

    bool load(const std::shared_ptr<Utils::MeshData>& meshData) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    if (!this->cubeParts[i][j][k]->load(meshData)) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    bool isCompleted();

    void shuffle(int times) {
        std::srand(std::time(0));

        for (int i = 0; i < times; i++) {
            int row = std::rand() / (RAND_MAX / 1.0f) * 3;
            int column = std::rand() / (RAND_MAX / 1.0f) * 3;

            this->selectSubCube(Math::Vec3(row, column, 0.0f));
            this->setState(static_cast<CubeState>(std::rand() / (RAND_MAX / 1.0f) * 4 + 1));
            this->animate(90.0f / this->rotationSpeed);
        }
    }

    void animate(float frameTime);

private:
    void rotateCubePresentation(CubeState state, const Math::Vec3 cubeArrayPosition);
    void rotateCubeMeshes(CubeState state, float angle, const Math::Vec3 cubeArrayPosition);

    bool isOrdered() const {
        int id = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    if (this->cubeParts[i][j][k]->getId() != id++) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    std::shared_ptr<CubePart> cubeParts[3][3][3];
    Math::Vec3 selectedCubePart;

    CubeState state;
    float rotationSpeed;
};

}  // namespace Game

}  // namespace Rubik

#endif  // CUBE_H
