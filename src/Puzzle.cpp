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
#include <ctime>

namespace Rubik {

const std::pair<int, int>& Puzzle::getSelectedCube() const {
    return this->selectedCube;
}

void Puzzle::selectCube(const std::pair<int, int>& selectedCube) {
    this->selectedCube = selectedCube;
}

bool Puzzle::isSelectionValid(const std::pair<int, int>& selectedCube) {
    bool isRowValid = (selectedCube.first >=0 && selectedCube.first <= 2);
    bool isColumnValid = (selectedCube.second >= 0 && selectedCube.second <= 2);
    return (isRowValid && isColumnValid);
}

AnimationState Puzzle::getAnimationState() const {
    return this->state;
}

void Puzzle::setAnimationState(AnimationState state) {
    this->state = state;
}

float Puzzle::getRotationSpeed() const {
    return this->rotationSpeed;
}

void Puzzle::setRotationSpeed(float rotationSpeed) {
    this->rotationSpeed = rotationSpeed;
}

void Puzzle::attachCube(std::shared_ptr<Graphene::SceneNode> cube, int cubeId) {
    if (cubeId >= 27) {
        throw std::runtime_error(Graphene::LogFormat("attachCube()"));
    }

    Cube* linearCubes = &this->cubes[0][0][0];
    linearCubes[cubeId] = std::make_pair(cube, cubeId);
}

void Puzzle::shuffle(int times) {
    for (int i = 0; i < times; i++) {
        this->selectCube(std::make_pair(std::rand() % 3, std::rand() % 3));
        this->setAnimationState(static_cast<AnimationState>(std::rand() % 4 + 1));
        this->animate(90.0f / this->getRotationSpeed());
    }
}

bool Puzzle::isCompleted() {
    bool completed = false;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!completed && this->isOrdered()) {
                completed = true;
            }

            for (int k = 0; k < 3; k++) {
                this->rotateFacet(std::make_pair(k, k), AnimationState::RIGHT_ROTATION);
            }
        }

        for (int k = 0; k < 3; k++) {
            this->rotateFacet(std::make_pair(k, k), AnimationState::UP_ROTATION);
        }
    }

    return completed;
}

void Puzzle::animate(float frameTime) {
    static std::pair<int, int> currentSelectedCube = this->getSelectedCube();
    static AnimationState currentState = this->state;

    if (currentState == AnimationState::IDLE && this->state != AnimationState::IDLE) {
        currentSelectedCube = this->getSelectedCube();
        currentState = this->state;
    }

    static float rotationAngle = 0.0f;
    float rotationDirection = 1.0f;
    float stepAngle = this->getRotationSpeed() * frameTime;

    switch (currentState) {
        case AnimationState::RIGHT_ROTATION:
        case AnimationState::DOWN_ROTATION:
            rotationDirection = -1.0f;
            // Fall through

        case AnimationState::LEFT_ROTATION:
        case AnimationState::UP_ROTATION:
            if (rotationAngle + stepAngle > 90.0f) {
                stepAngle = 90.0f - rotationAngle;
            }

            rotationAngle += stepAngle;
            stepAngle *= rotationDirection;

            if (Puzzle::isSelectionValid(currentSelectedCube)) {
                this->rotateEntities(currentSelectedCube, stepAngle, currentState);
                if (rotationAngle == 90.0f) {
                    this->rotateFacet(currentSelectedCube, currentState);
                }
            } else {
                for (int i = 0; i < 3; i++) {
                    this->rotateEntities(std::make_pair(i, i), stepAngle, currentState);
                    if (rotationAngle == 90.0f) {
                        this->rotateFacet(std::make_pair(i, i), currentState);
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

void Puzzle::rotateFacet(const std::pair<int, int>& selectedCube, AnimationState state) {
    int row = selectedCube.first;
    int column = selectedCube.second;

    for (int i = 0; i < 2; i++) {
        switch (state) {
            case AnimationState::LEFT_ROTATION:
                this->cubes[i][column][0].swap(this->cubes[2][column][i]);
                this->cubes[2][column][i].swap(this->cubes[2 - i][column][2]);
                this->cubes[2 - i][column][2].swap(this->cubes[0][column][2 - i]);
                break;

            case AnimationState::RIGHT_ROTATION:
                this->cubes[2 - i][column][2].swap(this->cubes[0][column][2 - i]);
                this->cubes[2][column][i].swap(this->cubes[2 - i][column][2]);
                this->cubes[i][column][0].swap(this->cubes[2][column][i]);
                break;

            case AnimationState::DOWN_ROTATION:
                this->cubes[row][i][0].swap(this->cubes[row][2][i]);
                this->cubes[row][2][i].swap(this->cubes[row][2 - i][2]);
                this->cubes[row][2 - i][2].swap(this->cubes[row][0][2 - i]);
                break;

            case AnimationState::UP_ROTATION:
                this->cubes[row][2 - i][2].swap(this->cubes[row][0][2 - i]);
                this->cubes[row][2][i].swap(this->cubes[row][(2 - i)][2]);
                this->cubes[row][i][0].swap(this->cubes[row][2][i]);
                break;

            default:
                break;
        }
    }
}

void Puzzle::rotateEntities(const std::pair<int, int>& selectedCube, float angle, AnimationState state) {
    int row = selectedCube.first;
    int column = selectedCube.second;

    switch (state) {
        case AnimationState::DOWN_ROTATION:
        case AnimationState::UP_ROTATION:
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    this->cubes[row][j][k].first->roll(angle);
                }
            }
            break;

        case AnimationState::LEFT_ROTATION:
        case AnimationState::RIGHT_ROTATION:
            for (int i = 0; i < 3; i++) {
                for (int k = 0; k < 3; k++) {
                    this->cubes[i][column][k].first->yaw(angle);
                }
            }
            break;

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
