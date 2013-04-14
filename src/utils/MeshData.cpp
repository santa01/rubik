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

#include "MeshData.h"
#include "Logger.h"

#include <sstream>
#include <algorithm>
#include <fstream>
#include <utility>

namespace Rubik {

namespace Utils {

// Parser is very simple, assume meshes are sane
bool MeshData::load(const std::string& name) {
    std::fstream file(name.c_str(), std::ios::in);
    if (!file.good()) {
        Logger::getInstance().log(Logger::LOG_ERROR, "Cannot open file `%s'", name.c_str());
        return false;
    }

    std::string line;
    std::string header;
    int lineNumber = 1;

    std::vector<float> vertices;
    std::vector<float> uvs;
    std::vector<int> faces;

    while (std::getline(file, line)) {
        std::stringstream values(line);
        values >> header;

        if (header == "#" || header == "") {
        } else if (header == "v") {
            float x, y, z;
            values >> x >> y >> z;
            this->pushBack(vertices, {x, y, z});
        } else if (header == "vt") {
            float x, y;
            values >> x >> y;
            this->pushBack(uvs, {x, y});
        } else if (header == "f") {
            int i, j, k;
            values >> i >> j >> k;
            this->pushBack(faces, {i, j, k});
        } else {
            Logger::getInstance().log(Logger::LOG_WARNING, "%s:%d: malformed line `%s'",
                    name.c_str(), lineNumber++, line.c_str());
        }
    }

    file.close();

    if (vertices.size() / 3 != uvs.size() / 2) {
        Logger::getInstance().log(Logger::LOG_ERROR, "%s: vertices and uvs numbers differ", name.c_str());
        return false;
    }

    if (*std::max_element(faces.begin(), faces.end()) > static_cast<int>(vertices.size() / 3) ||
            *std::min_element(faces.begin(), faces.end()) < 0) {
        Logger::getInstance().log(Logger::LOG_ERROR, "%s: vertex indexes exeed valid range", name.c_str());
        return false;
    }

    this->verticesData = std::move(std::unique_ptr<float[]>(new float[vertices.size() + uvs.size()]));
    this->convertToBuffer(this->verticesData, {vertices, uvs});
    this->verticesNumber = vertices.size() / 3;

    this->facesData = std::move(std::unique_ptr<int[]>(new int[faces.size()]));
    this->convertToBuffer(this->facesData, {faces});
    this->facesNumber = faces.size() / 3;

    return true;
}

}  // namespace Utils

}  // namespace Rubik
