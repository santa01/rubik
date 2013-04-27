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

#include "TextLabel.h"
#include "ImageTexture.h"

#include <SDL2/SDL_image.h>

namespace Rubik {

namespace Game {

void TextLabel::render() {
    if (this->textUpdated && this->font != nullptr) {
        int textWidth, textHeight;

        if (!this->text.empty()) {
            TTF_SizeUTF8(this->font, text.c_str(), &textWidth, &textHeight);
            this->quadMesh->scaleX(1.0f / this->textAspectRatio);
            this->textAspectRatio = textWidth / (textHeight / 1.0f);
            this->quadMesh->scaleX(this->textAspectRatio);

            SDL_Color color = {0, 0, 0, 0};
            SDL_Surface *textSurface = TTF_RenderUTF8_Blended(this->font, text.c_str(), color);
            std::dynamic_pointer_cast<Opengl::ImageTexture>(this->quadMesh->getTexture())->load(textSurface);
            SDL_FreeSurface(textSurface);
        } else {
            Uint32 data[] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
            SDL_Surface* dummyImage = SDL_CreateRGBSurfaceFrom(&data, 2, 2, 32, 4,
                    0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
            std::dynamic_pointer_cast<Opengl::ImageTexture>(this->quadMesh->getTexture())->load(dummyImage);
        }
    }

    this->quadMesh->render();
}

}  // namespace Game

}  // namespace Rubik
