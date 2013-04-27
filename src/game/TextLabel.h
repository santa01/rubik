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
#include "Scalable.h"
#include "Movable.h"
#include "Renderable.h"
#include "ImageTexture.h"

#include <SDL2/SDL_ttf.h>
#include <string>

namespace Rubik {

namespace Game {

class TextLabel: public Common::Movable, public Common::Scalable, public Common::Renderable {
public:
    TextLabel():
            quadMesh(new Opengl::Mesh()) {
        this->quadMesh->setTexture(std::shared_ptr<Opengl::ImageTexture>(new Opengl::ImageTexture()));
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
            this->textUpdated = true;
        }
    }

    const std::string& getText() const {
        return this->text;
    }

    void setFont(TTF_Font* font) {
        this->font = font;
    }

    const TTF_Font* getFont() const {
        return this->font;
    }

    using Common::Movable::setPosition;

    void setPosition(const Math::Vec3& position) {
        this->quadMesh->setPosition(position);
    }

    Math::Vec3 getPosition() const {
        return this->quadMesh->getPosition();
    }

    void scaleX(float factor) {
        this->quadMesh->scaleX(factor);
    }

    void scaleY(float factor) {
        this->quadMesh->scaleY(factor);
    }

    void scaleZ(float factor) {
        this->quadMesh->scaleZ(factor);
    }

    float getXFactor() const {
        return this->quadMesh->getXFactor();
    }

    float getYFactor() const {
        return this->quadMesh->getYFactor();
    }

    float getZFactor() const {
        return this->quadMesh->getZFactor();
    }

    std::shared_ptr<Opengl::RenderEffect>& getEffect() {
        return this->quadMesh->getEffect();
    }

    void setEffect(const std::shared_ptr<Opengl::RenderEffect>& effect) {
        this->quadMesh->setEffect(effect);
    }

    std::shared_ptr<Opengl::Texture>& getTexture() {
        return this->quadMesh->getTexture();
    }

    void setTexture(const std::shared_ptr<Opengl::Texture>& texture) {
        this->quadMesh->setTexture(texture);
    }

    void render();

    bool load(const std::shared_ptr<Utils::MeshData>& vertexData) {
        return this->quadMesh->load(vertexData);
    }

    void clear() {
        this->setText("");
    }

private:
    std::shared_ptr<Opengl::Mesh> quadMesh;
    TTF_Font* font;

    std::string text;
    bool textUpdated;
    float textAspectRatio;
};

}  // namespace Game

}  // namespace Rubik

#endif  // TEXTLABEL_H
