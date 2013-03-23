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

#ifndef VEC4_H
#define VEC4_H

#include "Vec3.h"

#include <cmath>

namespace Rubik {

namespace Math {

class Vec4 {
public:
    enum {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3
    };

    static const Vec4 ZERO;

    Vec4() {
        this->vector[X] = 0.0f;
        this->vector[Y] = 0.0f;
        this->vector[Z] = 0.0f;
        this->vector[W] = 1.0f;
    }

    Vec4(float x, float y, float z, float w) {
        this->vector[X] = x;
        this->vector[Y] = y;
        this->vector[Z] = z;
        this->vector[W] = w;
    }

    Vec4(const Vec3& vector, float w) {
        this->vector[X] = vector.get(Vec3::X);
        this->vector[Y] = vector.get(Vec3::Y);
        this->vector[Z] = vector.get(Vec3::Z);
        this->vector[W] = w;
    }

    Vec4 operator -(const Vec4& vector) const {
        Vec4 me(*this);
        return me -= vector;
    }

    Vec4 operator +(const Vec4& vector) const {
        Vec4 me(*this);
        return me += vector;
    }

    Vec4 operator *(float scalar) const {
        Vec4 me(*this);
        return me *= scalar;
    }

    Vec4& operator -=(const Vec4& vector) {
        this->vector[X] -= vector.get(X);
        this->vector[Y] -= vector.get(Y);
        this->vector[Z] -= vector.get(Z);
        this->vector[W] -= vector.get(W);
        return *this;
    }

    Vec4& operator +=(const Vec4& vector) {
        this->vector[X] += vector.get(X);
        this->vector[Y] += vector.get(Y);
        this->vector[Z] += vector.get(Z);
        this->vector[W] += vector.get(W);
        return *this;
    }

    Vec4& operator *=(float scalar) {
        this->vector[X] *= scalar;
        this->vector[Y] *= scalar;
        this->vector[Z] *= scalar;
        this->vector[W] *= scalar;
        return *this;
    }

    bool operator ==(const Vec4& vector) const {
        return (this->vector[X] == vector.get(X)) &&
               (this->vector[Y] == vector.get(Y)) &&
               (this->vector[Z] == vector.get(Z)) &&
               (this->vector[W] == vector.get(W));
    }

    bool operator !=(const Vec4& vector) const {
        return !(*this == vector);
    }

    Vec4 operator -() const {
        return Vec4(-this->vector[X],
                    -this->vector[Y],
                    -this->vector[Z],
                    -this->vector[W]);
    }

    float dot(const Vec4& vector) const {
        return this->vector[X] * vector.get(X) +
               this->vector[Y] * vector.get(Y) +
               this->vector[Z] * vector.get(Z) +
               this->vector[W] * vector.get(W);
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

    const float* data() const  {
        return (float*)&this->vector;
    }

    Vec3 extractVec3() const {
        return Vec3(this->vector[X],
                    this->vector[Y],
                    this->vector[Z]);
    }

private:
    float vector[4];
};

}  // namespace Math

}  // namespace Rubik

#endif  // VEC4_H
