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

#include <SDL2/SDL_image.h>
#include <Vec4.h>

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
    if (!this->text.empty() && this->font != nullptr) {
        int textWidth, textHeight;
        TTF_SizeUTF8(this->font.get(), text.c_str(), &textWidth, &textHeight);

        Math::Mat4 world(this->projection);
        world.invert();

        Math::Vec4 origin((world * this->ndc) * Math::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        Math::Vec4 size(textWidth, textHeight, 0.0f, 1.0f);
        size = (world * this->ndc) * size - origin;

        this->scaleX(1.0f / this->widthScaleFactor);
        this->widthScaleFactor = size.get(Math::Vec4::X);
        this->scaleX(this->widthScaleFactor);

        this->scaleY(1.0f / this->heightScaleFactor);
        this->heightScaleFactor = size.get(Math::Vec4::Y);
        this->scaleY(this->heightScaleFactor);

        SDL_Color color = { 0, 0, 0, 0 };
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(this->font.get(), text.c_str(), color);
        std::dynamic_pointer_cast<Opengl::ImageTexture>(this->getTexture())->load(textSurface);
        SDL_FreeSurface(textSurface);
    } else {
        Uint32 data[] = { 0x000000FF, 0x00000000, 0x00000000, 0x00000000 };
        SDL_Surface* dummyImage = SDL_CreateRGBSurfaceFrom(&data, 2, 2, 32, 4,
                0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
        std::dynamic_pointer_cast<Opengl::ImageTexture>(this->getTexture())->load(dummyImage);
        SDL_FreeSurface(dummyImage);
    }
}

}  // namespace Game

}  // namespace Rubik
