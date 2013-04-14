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

#ifndef MESHDATA_H
#define MESHDATA_H

#include <string>
#include <memory>
#include <vector>
#include <initializer_list>

namespace Rubik {

namespace Utils {

class MeshData {
public:
    MeshData() {
        this->verticesNumber = 0;
        this->facesNumber = 0;
    }

    int getVerticesNumber() const {
        return this->verticesNumber;
    }

    int getFacesNumber() const {
        return this->facesNumber;
    }

    const void* getVerticesData() const {
        return this->verticesData.get();
    }

    const void* getFacesData() const {
        return this->facesData.get();
    }

    bool load(const std::string& name);

private:
    template<typename T>
    void pushBack(std::vector<T>& vector, const std::initializer_list<T> args) const {
        for (T arg: args) {
            vector.push_back(arg);
        }
    }

    template<typename T>
    void convertToBuffer(const std::unique_ptr<T[]>& pointer,
            const std::initializer_list<std::vector<T>> vectors) const {
        int i = 0;
        for (auto& vector: vectors) {
            for (T value: vector) {
                pointer[i++] = value;
            }
        }
    }

    std::unique_ptr<float[]> verticesData;
    std::unique_ptr<int[]> facesData;

    int verticesNumber;
    int facesNumber;
};

}  // namespace Utils

}  // namespace Rubik

#endif  // MESHDATA_H
