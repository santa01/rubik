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

#include <Puzzle.h>
#include <Logger.h>

namespace Rubik {

const Math::Vec3 Puzzle::DUMMY_SELECTION(100.0f, 100.0f, 100.0f);

Puzzle::Puzzle() {
    std::srand(static_cast<int>(std::time(0)));
}

float Puzzle::getRotationSpeed() const {
    return this->rotationSpeed;
}

void Puzzle::setRotationSpeed(float rotationSpeed) {
    this->rotationSpeed = rotationSpeed;
}

const Math::Vec3& Puzzle::getSelectedCube() const {
    return this->selectedCube;
}

void Puzzle::selectCube(const Math::Vec3& cubePosition) {
    this->selectedCube = cubePosition;
}

AnimationState Puzzle::getAnimationState() const {
    return this->state;
}

void Puzzle::setAnimationState(AnimationState state) {
    this->state = state;
}

void Puzzle::attachCube(int i, int j, int k, std::shared_ptr<Graphene::SceneNode> cube) {
    if (i >= 3 || j >= 3 || k >= 3) {
        throw std::runtime_error(Graphene::LogFormat("setCube()"));
    }

    this->cubes[i][j][k] = std::make_pair(cube, i * j * k);
}

void Puzzle::shuffle(int times) {
    for (int i = 0; i < times; i++) {
        int row = std::rand() % 3;
        int column = std::rand() % 3;

        this->selectCube(Math::Vec3(static_cast<float>(row), static_cast<float>(column), 0.0f));
        this->setAnimationState(static_cast<AnimationState>(std::rand() % 4 + 1));
        this->animate(90.0f / this->rotationSpeed);
    }
}

bool Puzzle::isCompleted() {
    // TODO: Optimize this brute-force method
    bool completed = false;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!completed && this->isOrdered()) {
                completed = true;
            }

            for (int k = 0; k < 3; k++) {
                float coordinate = static_cast<float>(k);
                this->rotatePuzzleCubes(AnimationState::RIGHT_ROTATION, Math::Vec3(coordinate, coordinate, 0.0f));
            }
        }

        for (int k = 0; k < 3; k++) {
            float coordinate = static_cast<float>(k);
            this->rotatePuzzleCubes(AnimationState::UP_ROTATION, Math::Vec3(coordinate, coordinate, 0.0f));
        }
    }

    return completed;
}

void Puzzle::animate(float frameTime) {
    static float rotationAngle = 0.0f;
    static float stepAngle = 0.0f;

    static Math::Vec3 currentSelectedCube = this->selectedCube;
    static float currentRotationSpeed = this->rotationSpeed;
    static AnimationState currentState = this->state;

    if (currentState == AnimationState::IDLE && this->state != AnimationState::IDLE) {
        currentSelectedCube = this->selectedCube;
        currentRotationSpeed = this->rotationSpeed;
        currentState = this->state;
    }

    float signCorrection = 1.0f;

    switch (currentState) {
        case AnimationState::RIGHT_ROTATION:
        case AnimationState::DOWN_ROTATION:
            signCorrection = -1.0f;
            // Fall through

        case AnimationState::LEFT_ROTATION:
        case AnimationState::UP_ROTATION:
            if (rotationAngle + currentRotationSpeed * frameTime > 90.0f) {
                stepAngle = 90.0f - rotationAngle;
            } else {
                stepAngle = currentRotationSpeed * frameTime;
            }

            rotationAngle += stepAngle;

            if (currentSelectedCube != DUMMY_SELECTION) {
                this->rotatePuzzleEntities(currentState, stepAngle * signCorrection, currentSelectedCube);
                if (rotationAngle == 90.0f) {
                    this->rotatePuzzleCubes(currentState, currentSelectedCube);
                }
            } else {
                for (int i = 0; i < 3; i++) {
                    float coordinate = static_cast<float>(i);
                    this->rotatePuzzleEntities(currentState, stepAngle * signCorrection, Math::Vec3(coordinate, coordinate, 0.0f));
                    if (rotationAngle == 90.0f) {
                        this->rotatePuzzleCubes(currentState, Math::Vec3(coordinate, coordinate, 0.0f));
                    }
                }
            }

            if (rotationAngle == 90.0f) {
                this->state = AnimationState::IDLE;
                currentState = AnimationState::IDLE;
                rotationAngle = 0.0f;
            }
            break;

        default:
            break;
    }
}

void Puzzle::rotatePuzzleCubes(AnimationState state, const Math::Vec3 cubePosition) {
    for (int i = 0; i < 2; i++) {
        switch (state) {
            case AnimationState::LEFT_ROTATION: {
                int plane = static_cast<int>(cubePosition.get(Math::Vec3::Y));
                this->cubes[i][plane][0].swap(this->cubes[2][plane][i]);
                this->cubes[2][plane][i].swap(this->cubes[2 - i][plane][2]);
                this->cubes[2 - i][plane][2].swap(this->cubes[0][plane][2 - i]);
                break;
            }

            case AnimationState::RIGHT_ROTATION: {
                int plane = static_cast<int>(cubePosition.get(Math::Vec3::Y));
                this->cubes[2 - i][plane][2].swap(this->cubes[0][plane][2 - i]);
                this->cubes[2][plane][i].swap(this->cubes[2 - i][plane][2]);
                this->cubes[i][plane][0].swap(this->cubes[2][plane][i]);
                break;
            }

            case AnimationState::DOWN_ROTATION: {
                int plane = static_cast<int>(cubePosition.get(Math::Vec3::X));
                this->cubes[plane][i][0].swap(this->cubes[plane][2][i]);
                this->cubes[plane][2][i].swap(this->cubes[plane][2 - i][2]);
                this->cubes[plane][2 - i][2].swap(this->cubes[plane][0][2 - i]);
                break;
            }

            case AnimationState::UP_ROTATION: {
                int plane = static_cast<int>(cubePosition.get(Math::Vec3::X));
                this->cubes[plane][2 - i][2].swap(this->cubes[plane][0][2 - i]);
                this->cubes[plane][2][i].swap(this->cubes[plane][(2 - i)][2]);
                this->cubes[plane][i][0].swap(this->cubes[plane][2][i]);
                break;
            }

            default:
                break;
        }
    }
}

void Puzzle::rotatePuzzleEntities(AnimationState state, float angle, const Math::Vec3 cubePosition) {
    switch (state) {
        case AnimationState::DOWN_ROTATION:
        case AnimationState::UP_ROTATION: {
            int plane = static_cast<int>(cubePosition.get(Math::Vec3::X));
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    this->cubes[plane][j][k].first->roll(angle);
                }
            }
            break;
        }

        case AnimationState::LEFT_ROTATION:
        case AnimationState::RIGHT_ROTATION: {
            int plane = static_cast<int>(cubePosition.get(Math::Vec3::Y));
            for (int i = 0; i < 3; i++) {
                for (int k = 0; k < 3; k++) {
                    this->cubes[i][plane][k].first->yaw(angle);
                }
            }
            break;
        }

        default:
            break;
    }
}

bool Puzzle::isOrdered() const {
    int id = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                if (this->cubes[i][j][k].second != id++) {
                    return false;
                }
            }
        }
    }

    return true;
}

}  // namespace Rubik
