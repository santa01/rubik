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

#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "RenderEffect.h"
#include "Texture.h"

#include <memory>

namespace Rubik {

namespace Common {

class Renderable {
public:
    virtual ~Renderable() {}

    virtual std::shared_ptr<Opengl::RenderEffect>& getEffect() = 0;
    virtual void setEffect(const std::shared_ptr<Opengl::RenderEffect>& effect) = 0;

    virtual std::shared_ptr<Opengl::Texture>& getTexture() = 0;
    virtual void setTexture(const std::shared_ptr<Opengl::Texture>& texture) = 0;

    virtual void render() = 0;
};

}  // namespace Common

}  // namespace Rubik

#endif  // RENDERABLE_H
