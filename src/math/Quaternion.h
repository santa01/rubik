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

#ifndef QUATERNION_H
#define QUATERNION_H

#include "Vec3.h"
#include "Mat4.h"

#include <cmath>

namespace Rubik {

namespace Math {

class Quaternion {
public:
    enum {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3
    };

    Quaternion() {
        this->vector[X] = 0.0f;
        this->vector[Y] = 0.0f;
        this->vector[Z] = 0.0f;
        this->vector[W] = 1.0f;
    }

    Quaternion(float x, float y, float z, float w) {
        this->vector[X] = x;
        this->vector[Y] = y;
        this->vector[Z] = z;
        this->vector[W] = w;
    }

    Quaternion(const Vec3& axis, float angle) {
        float sinAngle = sinf(angle / 2);

        this->vector[X] = axis.get(Vec3::X) * sinAngle;
        this->vector[Y] = axis.get(Vec3::Y) * sinAngle;
        this->vector[Z] = axis.get(Vec3::Z) * sinAngle;
        this->vector[W] = cosf(angle / 2);
    }

    Quaternion operator *(const Quaternion& quaternmion) const;

    Quaternion& normalize() {
        float length = this->length();
        this->vector[X] /= length;
        this->vector[Y] /= length;
        this->vector[Z] /= length;
        this->vector[W] /= length;
        return *this;
    }

    float length() const {
        return sqrtf(this->vector[X] * this->vector[X] +
                     this->vector[Y] * this->vector[Y] +
                     this->vector[Z] * this->vector[Z] +
                     this->vector[W] * this->vector[W]);
    }

    float get(int index) const {
        switch (index) {
            case X:
            case Y:
            case Z:
            case W:
                return this->vector[index];

            default:
                return NAN;
        }
    }

    void set(int index, float value) {
        switch (index) {
            case X:
            case Y:
            case Z:
            case W:
                this->vector[index] = value;
                break;
        }
    }

    Mat4 extractMat4() const;

    void extractEulerAgngles(float& xAngle, float& yAngle, float& zAngle) const {
        xAngle = asinf(2 * (this->vector[X] * this->vector[Y] + this->vector[Z] * this->vector[W]));
        yAngle = atan2f(2 * (this->vector[Y] * this->vector[W] - this->vector[X] * this->vector[Z]),
                        1 - 2 * (this->vector[Y] * this->vector[Y] - this->vector[Z] * this->vector[Z]));
        zAngle = atan2f(2 * (this->vector[X] * this->vector[W] - this->vector[Y] * this->vector[Z]),
                        1 - 2 * (this->vector[X] * this->vector[X] - this->vector[Z] * this->vector[Z]));
    }

private:
    float vector[4];
};

}  // namespace Math

}  // namespace Rubik

#endif  // QUATERNION_H
