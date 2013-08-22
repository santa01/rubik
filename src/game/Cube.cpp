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
#include "Logger.h"

namespace Rubik {

namespace Game {

const Math::Vec3 Cube::DUMMY_SELECTION(100.0f, 100.0f, 100.0f);

bool Cube::isCompleted() {
    // TODO: Optimize this brute-force method
    bool completed = false;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!completed && this->isOrdered()) {
                completed = true;
            }

            for (int k = 0; k < 3; k++) {
                this->rotateCubePresentation(CubeState::STATE_RIGHT_ROTATION, Math::Vec3(k, k, 0.0f));
            }
        }

        for (int k = 0; k < 3; k++) {
            this->rotateCubePresentation(CubeState::STATE_UP_ROTATION, Math::Vec3(k, k, 0.0f));
        }
    }

    return completed;
}

void Cube::animate(float frameTime) {
    static float rotationAngle = 0.0f;
    static float stepAngle = 0.0f;

    static Math::Vec3 currentSelectedCubePart = this->selectedCubePart;
    static float currentRotationSpeed = this->rotationSpeed;
    static CubeState currentState = this->state;

    if (currentState == STATE_IDLE && this->state != STATE_IDLE) {
        currentSelectedCubePart = this->selectedCubePart;
        currentRotationSpeed = this->rotationSpeed;
        currentState = this->state;
    }

    float signCorrection = 1.0f;

    switch (currentState) {
        case STATE_RIGHT_ROTATION:
        case STATE_DOWN_ROTATION:
            signCorrection = -1.0f;
            // Fall through

        case STATE_LEFT_ROTATION:
        case STATE_UP_ROTATION:
            if (rotationAngle + currentRotationSpeed * frameTime > 90.0f) {
                stepAngle = 90.0f - rotationAngle;
            } else {
                stepAngle = currentRotationSpeed * frameTime;
            }

            rotationAngle += stepAngle;

            if (currentSelectedCubePart != DUMMY_SELECTION) {
                this->rotateCubeMeshes(currentState, stepAngle * signCorrection, currentSelectedCubePart);
                if (rotationAngle == 90.0f) {
                    this->rotateCubePresentation(currentState, currentSelectedCubePart);
                }
            } else {
                for (int i = 0; i < 3; i++) {
                    this->rotateCubeMeshes(currentState, stepAngle * signCorrection, Math::Vec3(i, i, 0.0f));
                    if (rotationAngle == 90.0f) {
                        this->rotateCubePresentation(currentState, Math::Vec3(i, i, 0.0f));
                    }
                }
            }

            if (rotationAngle == 90.0f) {
                this->state = STATE_IDLE;
                currentState = STATE_IDLE;
                rotationAngle = 0.0f;
            }
            break;

        default:
            break;
    }
}

void Cube::rotateCubePresentation(CubeState state, const Math::Vec3 cubeArrayPosition) {
    for (int i = 0; i < 2; i++) {
        int plane;

        switch (state) {
            case STATE_LEFT_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::Y);
                this->cubeParts[i][plane][0].swap(this->cubeParts[2][plane][i]);
                this->cubeParts[2][plane][i].swap(this->cubeParts[2 - i][plane][2]);
                this->cubeParts[2 - i][plane][2].swap(this->cubeParts[0][plane][2 - i]);
                break;

            case STATE_RIGHT_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::Y);
                this->cubeParts[2 - i][plane][2].swap(this->cubeParts[0][plane][2 - i]);
                this->cubeParts[2][plane][i].swap(this->cubeParts[2 - i][plane][2]);
                this->cubeParts[i][plane][0].swap(this->cubeParts[2][plane][i]);
                break;

            case STATE_DOWN_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::X);
                this->cubeParts[plane][i][0].swap(this->cubeParts[plane][2][i]);
                this->cubeParts[plane][2][i].swap(this->cubeParts[plane][2 - i][2]);
                this->cubeParts[plane][2 - i][2].swap(this->cubeParts[plane][0][2 - i]);
                break;

            case STATE_UP_ROTATION:
                plane = cubeArrayPosition.get(Math::Vec3::X);
                this->cubeParts[plane][2 - i][2].swap(this->cubeParts[plane][0][2 - i]);
                this->cubeParts[plane][2][i].swap(this->cubeParts[plane][(2 - i)][2]);
                this->cubeParts[plane][i][0].swap(this->cubeParts[plane][2][i]);
                break;

            default:
                break;
        }
    }
}

void Cube::rotateCubeMeshes(CubeState state, float angle, const Math::Vec3 cubeArrayPosition) {
    int plane;
    switch (state) {
        case STATE_DOWN_ROTATION:
        case STATE_UP_ROTATION:
            plane = cubeArrayPosition.get(Math::Vec3::X);
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    this->cubeParts[plane][j][k]->roll(angle);
                }
            }
            break;

        case STATE_LEFT_ROTATION:
        case STATE_RIGHT_ROTATION:
            plane = cubeArrayPosition.get(Math::Vec3::Y);
            for (int i = 0; i < 3; i++) {
                for (int k = 0; k < 3; k++) {
                    this->cubeParts[i][plane][k]->yaw(angle);
                }
            }
            break;

        default:
            break;
    }
}

}  // namespace Game

}  // namespace Rubik
