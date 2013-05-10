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

#include "ResourceManager.h"
#include "ShaderLoader.h"

#include <fstream>
#include <sstream>

namespace Rubik {

namespace Utils {

std::shared_ptr<Opengl::Texture>& ResourceManager::makeTexture(const std::string& name) {
    if (this->textureCache.find(name) == this->textureCache.end()) {
        Logger::getInstance().log(Logger::LOG_INFO, "Loading texture `%s'", name.c_str());

        SDL_Surface* image = IMG_Load(name.c_str());
        if (image == nullptr) {
            Logger::getInstance().log(Logger::LOG_ERROR, "IMG_Load(%s) failed: %s", name.c_str(), IMG_GetError());
            return this->textureCache["nullptr"];
        }

        this->insertTexture(name, image);
        SDL_FreeSurface(image);
    }

    return this->textureCache[name];
}

std::shared_ptr<Opengl::RenderEffect>& ResourceManager::makeEffect(const std::string& name) {
    if (this->effectCache.find(name) == this->effectCache.end()) {
        Logger::getInstance().log(Logger::LOG_INFO, "Loading shader `%s'", name.c_str());

        std::fstream file(name.c_str(), std::ios::binary | std::ios::in);
        if (!file.good()) {
            Logger::getInstance().log(Logger::LOG_ERROR, "Cannot open file `%s'", name.c_str());
            return this->effectCache["nullptr"];
        }

        file.seekg(0, std::ios::end);
        int sourceLength = file.tellg();

        std::unique_ptr<char[]> shaderSource(new char[sourceLength + 1]);
        shaderSource[sourceLength] = '\0';

        file.seekg(0, std::ios::beg);
        file.read(shaderSource.get(), sourceLength);
        file.close();

        this->insertEffect(name, shaderSource.get());
    }

    return this->effectCache[name];
}

std::shared_ptr<MeshData>& ResourceManager::makeMesh(const std::string& name) {
    if (this->meshDataCache.find(name) == this->meshDataCache.end()) {
        Logger::getInstance().log(Logger::LOG_INFO, "Loading mesh `%s'", name.c_str());

        std::shared_ptr<MeshData> vertexData(new MeshData());
        if (!vertexData->load(name)) {
            // VertexData::load() prints errors for us
            return this->meshDataCache["nullptr"];
        }

        this->meshDataCache.insert(std::make_pair(name, vertexData));
    }

    return this->meshDataCache[name];
}

std::shared_ptr<TTF_Font>& ResourceManager::makeFont(const std::string& name) {
    if (this->fontCache.find(name) == this->fontCache.end()) {
        Logger::getInstance().log(Logger::LOG_INFO, "Loading font `%s'", name.c_str());

        // Yep, its always 12pt sized
        std::shared_ptr<TTF_Font> font(TTF_OpenFont("fonts/dejavu-sans.ttf", 12), TTF_CloseFont);
        if (font == nullptr) {
            Logger::getInstance().log(Logger::LOG_ERROR, "TTF_OpenFont failed: %s", TTF_GetError());
            return this->fontCache["nullptr"];
        }

        this->fontCache.insert(std::make_pair(name, font));
    }

    return this->fontCache[name];
}

void ResourceManager::purgeCaches() {
    for (auto& texture: this->textureCache) {
        if (!texture.second.unique() && texture.second != nullptr) {
            Logger::getInstance().log(Logger::LOG_WARNING, "Texture %p has %d references left!",
                    texture.second.get(), texture.second.use_count() - 1);
        }
    }

    for (auto& effect: this->effectCache) {
        if (!effect.second.unique() && effect.second != nullptr) {
            Logger::getInstance().log(Logger::LOG_WARNING, "RenderEffect %p has %d references left!",
                    effect.second.get(), effect.second.use_count() - 1);
        }
    }

    for (auto& mesh: this->meshDataCache) {
        if (!mesh.second.unique() && mesh.second != nullptr) {
            Logger::getInstance().log(Logger::LOG_WARNING, "Mesh %p has %d references left!",
                    mesh.second.get(), mesh.second.use_count() - 1);
        }
    }

    for (auto& font: this->fontCache) {
        if (!font.second.unique() && font.second != nullptr) {
            Logger::getInstance().log(Logger::LOG_WARNING, "Font %p has %d references left!",
                    font.second.get(), font.second.use_count() - 1);
        }
    }

    this->textureCache.clear();
    this->effectCache.clear();
    this->meshDataCache.clear();
    this->fontCache.clear();
}

void ResourceManager::insertEffect(const std::string& name, const std::string& source) {
    std::shared_ptr<Opengl::RenderEffect> effect(new Opengl::RenderEffect());

    std::stringstream modifiedSource;
    modifiedSource << "#define TYPE_VERTEX\n";
    modifiedSource << source;
    effect->attachShader(modifiedSource.str(), Opengl::RenderEffect::ShaderType::TYPE_VERTEX);

    modifiedSource.str("");
    modifiedSource << "#define TYPE_FRAGMENT\n";
    modifiedSource << source;
    effect->attachShader(modifiedSource.str(), Opengl::RenderEffect::ShaderType::TYPE_FRAGMENT);

    effect->enable();  // Compile
    this->effectCache.insert(std::make_pair(name, effect));
}

}  // namespace Utils

}  // namespace Rubik
