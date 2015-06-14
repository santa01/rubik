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

#include "Label.h"

#include <Vec4.h>
#include <tuple>

namespace Rubik {

namespace Game {

Label::Label() {
    GLint viewportParameters[4];
    glGetIntegerv(GL_VIEWPORT, viewportParameters);

    this->ndc.set(0, 0, 2.0f / (viewportParameters[2] / 1.0f));
    this->ndc.set(0, 3, -1.0f);
    this->ndc.set(1, 1, 2.0f / (viewportParameters[3] / 1.0f));
    this->ndc.set(1, 3, -1.0f);

    this->setTexture(std::shared_ptr<Opengl::ImageTexture>(new Opengl::ImageTexture()));
    this->widthScaleFactor = 1.0f;
    this->heightScaleFactor = 1.0f;
}

void Label::renderText() {
    SDL_Surface* textSurface = nullptr;

    if (this->font == nullptr || this->text.empty()) {
        textSurface = SDL_CreateRGBSurface(0, 2, 2, 32,
            0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    } else {
        auto context = this->font->render(this->text);

        Math::Mat4 world(this->projection);
        world.invert();

        Math::Vec4 origin((world * this->ndc) * Math::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        Math::Vec4 size(context->width, context->height, 0.0f, 1.0f);
        size = (world * this->ndc) * size - origin;

        this->scaleX(1.0f / this->widthScaleFactor);
        this->widthScaleFactor = size.get(Math::Vec4::X);
        this->scaleX(this->widthScaleFactor);

        this->scaleY(1.0f / this->heightScaleFactor);
        this->heightScaleFactor = size.get(Math::Vec4::Y);
        this->scaleY(this->heightScaleFactor);

        textSurface = SDL_CreateRGBSurfaceFrom(context->pixels, context->width, context->height,
            32, 4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    }

    auto texture = std::dynamic_pointer_cast<Opengl::ImageTexture>(this->getTexture());
    texture->load(textSurface);

    SDL_FreeSurface(textSurface);
}

}  // namespace Game

}  // namespace Rubik
