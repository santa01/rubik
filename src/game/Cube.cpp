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

#include "Cube.h"

namespace Rubik {

namespace Game {

const Math::Vec3 Cube::DUMMY_SELECTION(100.0f, 100.0f, 100.0f);

Cube::Cube() {
    for (int i = 0; i < 27; i++) {
        auto subCube = std::shared_ptr<Opengl::CubeMesh>(new Opengl::CubeMesh());
        subCube->setPosition(i / 9 - 1, i % 9 / 3 - 1, i % 9 % 3 - 1);
        subCube->scale(0.5f);
        this->cubeArray[i] = subCube;
    }

    this->rotationSpeed = 300.0f;
    this->state = STATE_IDLE;
}

void Cube::rotateCubeArray(CubeState state, const Math::Vec3 cubeArrayPosition) {
    for (int i = 0; i < 2; i++) {
        int plane;

        switch (state) {
            case STATE_LEFT_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::Y);
                this->cubeArray[i * 9 + plane * 3 + 0].swap(this->cubeArray[2 * 9 + plane * 3 + i]);
                this->cubeArray[2 * 9 + plane * 3 + i].swap(this->cubeArray[(2 - i) * 9 + plane * 3 + 2]);
                this->cubeArray[(2 - i) * 9 + plane * 3 + 2].swap(this->cubeArray[0 * 9 + plane * 3 + (2 - i)]);
                break;

            case STATE_RIGHT_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::Y);
                this->cubeArray[(2 - i) * 9 + plane * 3 + 2].swap(this->cubeArray[0 * 9 + plane * 3 + (2 - i)]);
                this->cubeArray[2 * 9 + plane * 3 + i].swap(this->cubeArray[(2 - i) * 9 + plane * 3 + 2]);
                this->cubeArray[i * 9 + plane * 3 + 0].swap(this->cubeArray[2 * 9 + plane * 3 + i]);
                break;

            case STATE_DOWN_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::X);
                this->cubeArray[plane * 9 + i * 3 + 0].swap(this->cubeArray[plane * 9 + 2 * 3 + i]);
                this->cubeArray[plane * 9 + 2 * 3 + i].swap(this->cubeArray[plane * 9 + (2 - i) * 3 + 2]);
                this->cubeArray[plane * 9 + (2 - i) * 3 + 2].swap(this->cubeArray[plane * 9 + 0 * 3 + (2 - i)]);
                break;

            case STATE_UP_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::X);
                this->cubeArray[plane * 9 + (2 - i) * 3 + 2].swap(this->cubeArray[plane * 9 + 0 * 3 + (2 - i)]);
                this->cubeArray[plane * 9 + 2 * 3 + i].swap(this->cubeArray[plane * 9 + (2 - i) * 3 + 2]);
                this->cubeArray[plane * 9 + i * 3 + 0].swap(this->cubeArray[plane * 9 + 2 * 3 + i]);
                break;

            default:
                break;
        }
    }
}

void Cube::render() {
    auto& renderEffect = this->getEffect();
    Math::Vec3 cubeArrayPosition;

    for (int i = 0; i < 27; i++) {
        auto& subCube = this->cubeArray[i];

        if (renderEffect != nullptr) {
            cubeArrayPosition = Math::Vec3(i / 9, i % 9 / 3, i % 9 % 3);
            renderEffect->setUniform("row", cubeArrayPosition);
            renderEffect->setUniform("cubeArrayPosition", cubeArrayPosition);
        }

        subCube->render();
    }
}

void Cube::animate(float frameTime) {
    static float rotationAngle = 0.0f;
    static float stepAngle = 0.0f;
    float signCorrection = 1.0f;

    switch (this->state) {
        case STATE_RIGHT_ROTATION:
        case STATE_DOWN_ROTATION:
            signCorrection = -1.0f;

        case STATE_LEFT_ROTATION:
        case STATE_UP_ROTATION:
            if (rotationAngle + this->rotationSpeed * frameTime > 90.0f) {
                stepAngle = 90.0f - rotationAngle;
            } else {
                stepAngle = this->rotationSpeed * frameTime;
            }

            rotationAngle += stepAngle;

            if (this->state == STATE_LEFT_ROTATION || this->state == STATE_RIGHT_ROTATION) {
                if (this->selectedSubCube != DUMMY_SELECTION) {
                    this->yaw(stepAngle * signCorrection, this->selectedSubCube);
                } else {
                    for (int i = 0; i < 3; i++) {
                        this->yaw(stepAngle * signCorrection, Math::Vec3(i, i, 0.0f));
                    }
                }
            } else {
                if (this->selectedSubCube != DUMMY_SELECTION) {
                    this->roll(stepAngle * signCorrection, this->selectedSubCube);
                } else {
                    for (int i = 0; i < 3; i++) {
                        this->roll(stepAngle * signCorrection, Math::Vec3(i, i, 0.0f));
                    }
                }
            }

            if (rotationAngle == 90.0f) {
                if (this->selectedSubCube != DUMMY_SELECTION) {
                    this->rotateCubeArray(state, this->selectedSubCube);
                } else {
                    for (int i = 0; i < 3; i++) {
                        this->rotateCubeArray(state, Math::Vec3(i, i, 0.0f));
                    }
                }

                this->state = STATE_IDLE;
                rotationAngle = 0.0f;
            }
            break;

        default:
            break;
    }
}

void Cube::roll(float angle, const Math::Vec3 cubeArrayPosition) {
    switch (state) {
        case STATE_DOWN_ROTATION:
        case STATE_UP_ROTATION:
            for (int i = 0; i < 27; i++) {
                if (i / 9 == cubeArrayPosition.get(Math::Vec3::X)) {
                    this->cubeArray[i]->roll(angle);
                }
            }
            break;

        default:
            break;
    }
}

void Cube::yaw(float angle, const Math::Vec3 cubeArrayPosition) {
    switch (state) {
        case STATE_LEFT_ROTATION:
        case STATE_RIGHT_ROTATION:
            for (int i = 0; i < 27; i++) {
                if (i % 9 / 3 == cubeArrayPosition.get(Math::Vec3::Y)) {
                    this->cubeArray[i]->yaw(angle);
                }
            }
            break;

        default:
            break;
    }
}

}  // namespace Game

}  // namespace Rubik
