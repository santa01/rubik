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
#include <SceneNode.h>
#include <Logger.h>
#include <Vec3.h>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

namespace Rubik {

int Puzzle::getSelectedCube() const {
    return this->selectedCube;
}

void Puzzle::selectCube(int objectId) {
    this->selectedCube = objectId;
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

void Puzzle::attachCube(const std::shared_ptr<Graphene::Entity> cube) {
    if (this->attachedCubes >= 27) {
        throw std::runtime_error(Graphene::LogFormat("attachCube()"));
    }

    std::shared_ptr<Graphene::Entity>* cubes = &this->cubes[0][0][0];
    std::shared_ptr<Graphene::Entity>* solution = &this->solution[0][0][0];

    cubes[this->attachedCubes] = cube;
    solution[this->attachedCubes] = cube;
    this->attachedCubes++;
}

std::tuple<int, int, int> Puzzle::getCubePosition(int objectId) const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                if (this->cubes[i][j][k]->getId() == objectId) {
                    return std::make_tuple(i, j, k);
                }
            }
        }
    }

    return std::make_tuple(-1, -1, -1);
}

void Puzzle::shuffle(int times) {
    for (int i = 0; i < times; i++) {
        this->selectedCube = this->cubes[std::rand() % 3][std::rand() % 3][std::rand() % 3]->getId();
        this->state = static_cast<AnimationState>(std::rand() % 4 + 1);
        this->update(90.0f / this->rotationSpeed);
    }
}

bool Puzzle::isSolved() {
    bool solved = false;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!solved && this->isOrdered()) {
                solved = true;
            }

            for (int k = 0; k < 3; k++) {
                this->rotateFacet(k, k, AnimationState::RIGHT_ROTATION);
            }
        }

        for (int k = 0; k < 3; k++) {
            this->rotateFacet(k, k, AnimationState::UP_ROTATION);
        }
    }

    return solved;
}

void Puzzle::update(float frameTime) {
    static int selectedCube = this->selectedCube;
    static AnimationState state = this->state;

    if (state == AnimationState::IDLE && this->state != AnimationState::IDLE) {
        selectedCube = this->selectedCube;
        state = this->state;
    }

    static float rotationAngle = 0.0f;
    float rotationDirection = 1.0f;
    float stepAngle = this->rotationSpeed * frameTime;

    switch (state) {
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

            if (selectedCube != -1) {
                std::tuple<int, int, int> cubePosition(this->getCubePosition(selectedCube));
                this->rotateEntities(std::get<0>(cubePosition), std::get<1>(cubePosition), stepAngle, state);
                if (rotationAngle == 90.0f) {
                    this->rotateFacet(std::get<0>(cubePosition), std::get<1>(cubePosition), state);
                }
            } else {
                for (int i = 0; i < 3; i++) {
                    this->rotateEntities(i, i, stepAngle, state);
                    if (rotationAngle == 90.0f) {
                        this->rotateFacet(i, i, state);
                    }
                }
            }

            if (rotationAngle == 90.0f) {
                this->state = AnimationState::IDLE;
                state = AnimationState::IDLE;
                rotationAngle = 0.0f;
            }
            break;

        default:
            break;
    }
}

void Puzzle::rotateFacet(int row, int column, AnimationState state) {
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

void Puzzle::rotateEntities(int row, int column, float angle, AnimationState state) {
    switch (state) {
        case AnimationState::DOWN_ROTATION:
        case AnimationState::UP_ROTATION:
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    // Rotate the parent SceneNode
                    this->cubes[row][j][k]->getParent()->rotate(Math::Vec3::UNIT_X, angle);
                }
            }
            break;

        case AnimationState::LEFT_ROTATION:
        case AnimationState::RIGHT_ROTATION:
            for (int i = 0; i < 3; i++) {
                for (int k = 0; k < 3; k++) {
                    // Rotate the parent SceneNode
                    this->cubes[i][column][k]->getParent()->rotate(Math::Vec3::UNIT_Y, angle);
                }
            }
            break;

        default:
            break;
    }
}

bool Puzzle::isOrdered() const {
    const std::shared_ptr<Graphene::Entity>* cubes = &this->cubes[0][0][0];
    const std::shared_ptr<Graphene::Entity>* solution = &this->solution[0][0][0];

    return std::equal(cubes, cubes + this->attachedCubes, solution);
}

}  // namespace Rubik
