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

#ifndef PUZZLE_H
#define PUZZLE_H

#include <NonCopyable.h>
#include <Entity.h>
#include <tuple>

namespace Rubik {

enum class AnimationState { IDLE, LEFT_ROTATION, RIGHT_ROTATION, UP_ROTATION, DOWN_ROTATION };

class Puzzle: public Graphene::NonCopyable {
public:
    int getSelectedCube() const;
    void selectCube(int objectId);

    AnimationState getAnimationState() const;
    void setAnimationState(AnimationState state);

    float getRotationSpeed() const;
    void setRotationSpeed(float rotationSpeed);

    void attachCube(const std::shared_ptr<Graphene::Entity> cube);
    std::tuple<int, int, int> getCubePosition(int objectId) const;

    void shuffle(int times);
    bool isSolved();

    void animate(float frameTime);

private:
    void rotateFacet(int row, int column, AnimationState state);
    void rotateEntities(int row, int column, float angle, AnimationState state);
    bool isOrdered() const;

    std::shared_ptr<Graphene::Entity> cubes[3][3][3];
    std::shared_ptr<Graphene::Entity> solution[3][3][3];
    int attachedCubes = 0;
    int selectedCube = -1;

    AnimationState state = AnimationState::IDLE;
    float rotationSpeed = 300.0f;
};

}  // namespace Rubik

#endif  // PUZZLE_H
