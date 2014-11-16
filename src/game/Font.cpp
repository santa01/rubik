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

#include "Font.h"

#include <ft2build.h>
#include FT_OUTLINE_H
#include FT_TYPES_H
#include <cstring>
#include <cstdint>

namespace Rubik {

namespace Game {

std::shared_ptr<FT_Library> Font::library;

void FT_Delete_Library(FT_Library* library) {
    FT_Done_FreeType(*library);
    delete library;
}

void FT_Delete_Face(FT_Face* face) {
    FT_Done_Face(*face);
    delete face;
}

void FT_Render_Callback(int y, int count, const FT_Span* spans, void* param) {
    auto context = static_cast<FT_RenderContext*>(param);
    auto pixels = static_cast<uint32_t*>(context->pixels);

    y = context->height - y - 1;
    for (int i = 0; i < count; i++) {
        FT_Span span = spans[i];
        uint32_t pixel = span.coverage << 24;

        for (int j = 0; j < span.len; j++) {
            pixels[y * context->width + span.x + j + context->offset] = pixel;
        }
    }
}

bool Font::load(const std::string& name, int size) {
    this->face = std::shared_ptr<FT_Face>(new FT_Face, FT_Delete_Face);
    if (FT_New_Face(*Font::library, name.c_str(), 0, this->face.get())) {
        return false;
    }

    FT_F26Dot6 fontSize = size * 64;
    if (FT_Set_Char_Size(*this->face, fontSize, fontSize, 72, 72)) {
        return false;
    }

    return true;
}

std::tuple<std::shared_ptr<void>, int, int> Font::render(const std::string &text) {
    FT_Face faceRec = *this->face;
    FT_GlyphSlot glyph = faceRec->glyph;
    FT_RenderContext context = { nullptr, 0, 0, 0 };

    for (char ch: text) {
        if (FT_Load_Char(*this->face, ch, FT_LOAD_DEFAULT)) {
            continue;
        }

        context.width += glyph->metrics.horiAdvance / 64;
        context.height = glyph->metrics.vertAdvance / 64;
    }

    int imageSize = context.width * context.height;
    context.pixels = new uint32_t[imageSize];
    std::memset(context.pixels, 0, sizeof(uint32_t) * imageSize);

    FT_Raster_Params rasterParams;
    std::memset(&rasterParams, 0, sizeof(rasterParams));

    rasterParams.flags = FT_RASTER_FLAG_DIRECT | FT_RASTER_FLAG_AA;
    rasterParams.gray_spans = &FT_Render_Callback;
    rasterParams.user = &context;

    for (char ch: text) {
        if (FT_Load_Char(*this->face, ch, FT_LOAD_DEFAULT)) {
            return std::make_tuple(nullptr, 0, 0);
        }

        if (FT_Outline_Render(*Font::library, &glyph->outline, &rasterParams)) {
            return std::make_tuple(nullptr, 0, 0);
        }

        context.offset += glyph->metrics.horiAdvance / 64;
    }

    std::shared_ptr<void> pixels(context.pixels);
    return std::make_tuple(pixels, context.width, context.height);
}

}  // namespace Game

}  // namespace Rubik
