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

#ifndef LABEL_H
#define LABEL_H

#include "Mesh.h"
#include "ImageTexture.h"
#include "Font.h"

#include <Mat4.h>
#include <string>
#include <memory>

namespace Rubik {

namespace Game {

class Label: public Opengl::Mesh {
public:
    Label();

    Label(const std::string& text):
            Label() {
        this->setText(text);
    }

    void setText(const std::string& text) {
        if (this->text != text) {
            this->text = text;
            this->renderText();
        }
    }

    const std::string& getText() const {
        return this->text;
    }

    void setFont(const std::shared_ptr<Font>& font) {
        if (this->font != font) {
            this->font = font;
            this->renderText();
        }
    }

    const std::shared_ptr<Font>& getFont() const {
        return this->font;
    }

    void setProjection(const Math::Mat4& projection) {
        if (this->projection != projection) {
            this->projection = projection;
            this->renderText();
        }
    }

    const Math::Mat4& getProjection() const {
        return this->projection;
    }

private:
    void renderText();

    std::shared_ptr<Font> font;
    std::string text;

    Math::Mat4 projection;
    Math::Mat4 ndc;

    float widthScaleFactor;
    float heightScaleFactor;
};

}  // namespace Game

}  // namespace Rubik

#endif  // LABEL_H
