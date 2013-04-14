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

#include "ImageTexture.h"

namespace Rubik {

namespace Opengl {

bool ImageTexture::load(SDL_Surface* image) {
    if (image == nullptr) {
        return false;
    }

    SDL_Surface* source = (image->format->BytesPerPixel != 4) ? this->convertToRGBA(image) : image;
    if (source == nullptr) {
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, source->w, source->h, 0,
            (source->format->Rmask > source->format->Bmask) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, source->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (source != image) {  // Free only our copy if made
        SDL_FreeSurface(source);
    }

    return true;
}

SDL_Surface* ImageTexture::convertToRGBA(SDL_Surface* image) {
    SDL_Surface* newSource = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 32,
            0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    if (newSource == nullptr) {
        return nullptr;
    }

    if (SDL_BlitSurface(image, nullptr, newSource, nullptr)) {
        SDL_FreeSurface(newSource);
        return nullptr;
    }

    return newSource;
}

}  // namespace Opengl

}  // namespace Rubik
