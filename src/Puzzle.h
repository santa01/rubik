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
#include <SceneNode.h>
#include <utility>
#include <ctime>

namespace Rubik {

enum class AnimationState {
    IDLE,
    LEFT_ROTATION,
    RIGHT_ROTATION,
    UP_ROTATION,
    DOWN_ROTATION
};

class Puzzle: public Graphene::NonCopyable {
public:
    Puzzle();

    float getRotationSpeed() const;
    void setRotationSpeed(float rotationSpeed);

    const Math::Vec3& getSelectedCube() const;
    void selectCube(const Math::Vec3& cubePosition);

    AnimationState getAnimationState() const;
    void setAnimationState(AnimationState state);

    void attachCube(int i, int j, int k, std::shared_ptr<Graphene::SceneNode> cube);
    bool isCompleted();

    void shuffle(int times);
    void animate(float frameTime);

private:
    void rotatePuzzleCubes(AnimationState state, const Math::Vec3 cubePosition);
    void rotatePuzzleEntities(AnimationState state, float angle, const Math::Vec3 cubePosition);

    bool isOrdered() const;

    static const Math::Vec3 DUMMY_SELECTION;

    std::pair<std::shared_ptr<Graphene::SceneNode>, int> cubes[3][3][3];
    Math::Vec3 selectedCube = Math::Vec3::ZERO;

    AnimationState state = AnimationState::IDLE;
    float rotationSpeed = 300.0f;
};

}  // namespace Rubik

#endif  // PUZZLE_H
