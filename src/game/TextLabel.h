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

#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include "Mesh.h"
#include "ImageTexture.h"

#include <SDL2/SDL_ttf.h>
#include <string>

namespace Rubik {

namespace Game {

class TextLabel: public Opengl::Mesh {
public:
    TextLabel() {
        this->setTexture(std::shared_ptr<Opengl::ImageTexture>(new Opengl::ImageTexture()));
        this->font = nullptr;
        this->textAspectRatio = 1.0f;
    }

    TextLabel(const std::string& text):
            TextLabel() {
        this->setText(text);
    }

    void setText(const std::string& text) {
        if (this->text != text) {
            this->text = text;

            if (this->font != nullptr) {
                this->renderText();
            }
        }
    }

    const std::string& getText() const {
        return this->text;
    }

    void setFont(TTF_Font* font) {
        this->font = font;
        this->renderText();
    }

    const TTF_Font* getFont() const {
        return this->font;
    }

    void clear() {
        this->setText("");
    }

private:
    void renderText();

    TTF_Font* font;
    std::string text;

    float textAspectRatio;
};

}  // namespace Game

}  // namespace Rubik

#endif  // TEXTLABEL_H
