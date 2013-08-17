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

#ifndef MAT3_H
#define MAT3_H

#include "Vec3.h"

#include <cmath>
#include <algorithm>

namespace Rubik {

namespace Math {

class Mat3 {
public:
    Mat3() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                this->matrix[i][j] = 0.0f;
            }
        }

        this->matrix[0][0] = 1.0f;
        this->matrix[1][1] = 1.0f;
        this->matrix[2][2] = 1.0f;
    }

    Mat3 operator *(const Mat3& matrix) const {
        Mat3 result;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.set(i, j, this->matrix[i][0] * matrix.get(0, j) +
                                 this->matrix[i][1] * matrix.get(1, j) +
                                 this->matrix[i][2] * matrix.get(2, j));
            }
        }

        return result;
    }

    Vec3 operator *(const Vec3& vector) const {
        Vec3 result;

        for (int i = 0; i < 3; i++) {
            result.set(i, this->matrix[i][0] * vector.get(Vec3::X) +
                          this->matrix[i][1] * vector.get(Vec3::Y) +
                          this->matrix[i][2] * vector.get(Vec3::Z));
        }

        return result;
    }

    Mat3 operator *(float scalar) const {
        Mat3 result;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.set(i, j, this->matrix[i][j] * scalar);
            }
        }

        return result;
    }

    Mat3 operator +(const Mat3& matrix) const {
        Mat3 result;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.set(i, j, this->matrix[i][j] + matrix.get(i, j));
            }
        }

        return result;
    }

    Mat3 operator -(const Mat3& matrix) const {
        Mat3 result;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.set(i, j, this->matrix[i][j] - matrix.get(i, j));
            }
        }

        return result;
    }

    bool operator ==(const Mat3& matrix) const {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (this->matrix[i][j] != matrix.get(i, j)) {
                    return false;
                }
            }
        }

        return true;
    }

    bool operator !=(const Mat3& matrix) const {
        return !(*this == matrix);
    }

    void transpose() {
        for (int i = 0; i < 2; i++) {
            for (int j = i + 1; j < 3; j++) {
                std::swap(this->matrix[j][i], this->matrix[i][j]);
            }
        }
    }

    /*
     * Doolitle LU decomposition method
     * References:
     *   http://www.engr.colostate.edu/~thompson/hPage/CourseMat/Tutorials/CompMethods/doolittle.pdf
     *   https://vismor.com/documents/network_analysis/matrix_algorithms/S4.SS2.php
     *   http://en.wikipedia.org/wiki/LU_decomposition#Doolittle_algorithm
     */
    void decompose(Mat3& lower, Mat3& upper) const;

    /*
     * Finds inverse matrix by LU-decomposition and further equation solving:
     * L * Z = I
     * U * X = Z
     * L * U * X = I
     * where:
     *   L and U are lower and upper triangular matrices got by Mat3::decompose(),
     *   X and I are respective columns of inverse and identity matrices.
     */
    Mat3& invert();

    /*
     * Performs forward substitution for lower triangular marix.
     * Matrix is assumed to be triangular, no check is performed.
     * References:
     *   http://en.wikipedia.org/wiki/Triangular_matrix#Forward_substitution
     */
    Vec3 solveL(const Vec3& absolute) const {
        Vec3 solution;

        for (int i = 0; i < 3; i++) {
            solution.set(i, absolute.get(i));
            for (int j = 0; j < i; j++) {
                solution.set(i, solution.get(i) - this->matrix[i][j] * solution.get(j));
            }
            solution.set(i, solution.get(i) / this->matrix[i][i]);
        }

        return solution;
    }

    /*
     * Performs backward substitution for upper triangular marix.
     * Matrix is assumed to be triangular, no check is performed.
     * References:
     *   http://en.wikipedia.org/wiki/Triangular_matrix#Forward_substitution
     */
    Vec3 solveU(const Vec3& absolute) const {
        Vec3 solution;

        for (int i = 2; i > -1; i--) {
            solution.set(i, absolute.get(i));
            for (int j = 2; j > i; j--) {
                solution.set(i, solution.get(i) - this->matrix[i][j] * solution.get(j));
            }
            solution.set(i, solution.get(i) / this->matrix[i][i]);
        }

        return solution;
    }

    float get(int row, int column) const {
        if (column < 0 || column > 2 || row < 0 || row > 2) {
            return NAN;
        }

        return this->matrix[row][column];
    }

    void set(int row, int column, float value) {
        if (column < 0 || column > 2 || row < 0 || row > 2) {
            return;
        }

        this->matrix[row][column] = value;
    }

    const float* data() const  {
        return (float*)&this->matrix;
    }

private:
    float matrix[3][3];
};

}  // namespace Math

}  // namespace Rubik

#endif  // MAT3_H
