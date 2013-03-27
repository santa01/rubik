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
#include "ResourceManager.h"

namespace Rubik {

namespace Game {

Cube::Cube() {
    for (int i = 0; i < static_cast<int>(this->cubeArray.size()); i++) {
        auto subCube = std::unique_ptr<Opengl::CubeMesh>(new Opengl::CubeMesh());
        subCube->setPosition(i / 9 - 1, i % 9 / 3 - 1, i % 9 % 3 - 1);
        subCube->setId(i);
        subCube->scale(0.5f);
        this->cubeArray[i] = std::move(subCube);
    }

    //this->yaw(180.0f);
    this->state = STATE_IDLE;
}

void Cube::animate(float frameTime) {
    static float rotationAngle = 0.0f;
    static float stepAngle = 0.0f;
    float signCorrection = 1.0f;

    switch (this->state) {
        case STATE_RIGHT_ROTATION:
        case STATE_UP_ROTATION:
            signCorrection = -1.0f;

        case STATE_LEFT_ROTATION:
        case STATE_DOWN_ROTATION:
            if (rotationAngle + 250.0f * frameTime > 90.0f) {
                stepAngle = 90.0f - rotationAngle;
            } else {
                stepAngle = 250.0f * frameTime;
            }
            rotationAngle += stepAngle;

            if (this->state == STATE_LEFT_ROTATION || this->state == STATE_RIGHT_ROTATION) {
                this->yaw(stepAngle * signCorrection);
            } else {
                this->roll(stepAngle * signCorrection);
            }

            if (rotationAngle == 90.0f) {
                this->state = STATE_IDLE;
                rotationAngle = 0.0f;
            }
            break;

        default:
            break;
    }
}

}  // namespace Game

}  // namespace Rubik
